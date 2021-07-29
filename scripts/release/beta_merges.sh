#!/bin/bash
set -euo pipefail

# Be sure to run the project in an empty directory for the first time.

# This script will fetch every open issue on recalbox/recalbox that has the label Testing::Beta
# For each issue it will get related merge requests, fetch the remote, and proceed to a merge in the beta branch of the project
# Once branches are merged, it will change subprojects versions in recalbox packages.

## TODO: Add pipeline state
## TODO: wait for user input when conflict

if [ -z "${1:-}" ]; then
    echo "Usage : bash beta_merges.sh GITLAB_TOKEN [MERGE] [DEBUG]
    Show info: bash beta_merges.sh XXXXX
    Show infos and merge : bash beta_merges.sh XXXXX 1
    Show infos and merge and debug : bash beta_merges.sh XXXXX 1 1" && exit 1
fi

TOKEN="${1}"
MERGE="${2:-0}"
DEBUG="${3:-0}"

CURDIR=$(pwd)

rm -f */.stamp_reseted

function get_add_fetch_remote() {
    project="${1}"
    source_project_id="${2}"
    source_branch="${3}"
    tabs="${4}"
    project_info=$(curl -sf --header "PRIVATE-TOKEN: ${TOKEN}" "https://gitlab.com/api/v4/projects/${source_project_id}" | jq)

    ssh_url_to_repo=$( echo "${project_info}" | jq -r ".ssh_url_to_repo" )
    namespace=$( echo "${project_info}" | jq -r ".namespace.full_path" )

    cd "${CURDIR}"
    if [ ! -d "${CURDIR}/${project}" ];then
        echo -e "${tabs}Cloning project git@gitlab.com:recalbox/${project}.git to ${project}"
        git clone "git@gitlab.com:recalbox/${project}.git" 2>&1 | sed "s/^/${tabs}/"
        cd "${project}"
        (git remote add recalbox "git@gitlab.com:recalbox/${project}.git" 2>&1 | sed "s/^/${tabs}/") || true
        cd "${CURDIR}"
    fi
    cd "${CURDIR}/${project}"
    if [[ ! $(git remote get-url "${namespace}") ]];then
        echo -e "${tabs}Adding remote ${namespace} on project ${project}"
        git remote add "${namespace}" "${ssh_url_to_repo}" | sed "s/^/${tabs}/"
        git fetch -p "${namespace}" | sed "s/^/${tabs}/"
    fi
    if [ ! -f "${CURDIR}/${project}/.stamp_reseted" ];then
        echo -e "${tabs}Reseting beta branches on project ${project}"
        git fetch recalbox -p | sed "s/^/${tabs}/"
        git reset --hard | sed "s/^/${tabs}/"
        (git checkout -b beta recalbox/beta > /dev/null 2>&1 ) || (git checkout -b beta > /dev/null 2>&1 | sed "s/^/${tabs}/") || (git checkout beta > /dev/null 2>&1 | sed "s/^/${tabs}/")
        git reset --hard recalbox/master | sed "s/^/${tabs}/"
        echo "1" > "${CURDIR}/${project}/.stamp_reseted"
    fi

    CURRENT_REMOTE="${namespace}"
    cd "${CURDIR}"
}



echo -e "\n--------------------------------------------
Checking all issues with label Testing::Beta
--------------------------------------------"

ISSUES=$(curl -sf --header "PRIVATE-TOKEN: ${TOKEN}" "https://gitlab.com/api/v4/groups/recalbox/issues?labels=Testing::Beta&state=opened" | jq -c '.[]')

[ $DEBUG -eq 1 ] && echo "$ISSUES"

while IFS= read -r issue; do
    # Each issue will be inspected
    project_id=$( echo "${issue}" | jq ".project_id" )
    issue_id=$( echo "${issue}" | jq -r ".iid" )
    issue_title=$( echo "${issue}" | jq -r ".title" )
    issue_link=$( echo "${issue}" | jq -r ".web_url" )
    issue_ref=$( echo "${issue}" | jq -r ".references.full" )
    issue_assignee=$( echo "${issue}" | jq -r ".assignee.username" )

    # Getting current issue related merge requests
    MERGE_REQUESTS=$(curl -sf --header "PRIVATE-TOKEN: ${TOKEN}" "https://gitlab.com/api/v4/projects/${project_id}/issues/${issue_id}/related_merge_requests?state=opened" | jq -c '.[]')
    mr_count=$( echo "${MERGE_REQUESTS}" | wc -l& )

    echo -e "\n\n\e[32mIssue\e[0m ${issue_ref} assigneed to ${issue_assignee}\n\tTitle: ${issue_title}\n\tTotal merge requests (open and closed): ${mr_count}\n\tLink: ${issue_link}"
    [ $DEBUG -eq 1 ] && echo "${MERGE_REQUESTS}"

    while IFS= read -r mr; do
        # Each Merge Request on the issue may be merged

        # Don't merge already merged issues
        mr_state=$( echo "${mr}" | jq -r ".state" )
        if [[ "${mr_state}" != "opened" ]]; then
            continue
        fi
        mr_title=$( echo "${mr}" | jq -r ".title" )
        mr_ref=$( echo "${mr}" | jq -r ".references.full" )
        mr_relative_ref=$( echo "${mr}" | jq -r ".references.relative" )
        mr_assignee=$( echo "${mr}" | jq -r ".assignee.username" )
        mr_link=$( echo "${mr}" | jq -r ".web_url" )
        mr_source_branch=$( echo "${mr}" | jq -r ".source_branch" )
        mr_source_project_id=$( echo "${mr}" | jq -r ".source_project_id" )
        echo -e "\t\e[34mMerge request\e[0m ${mr_ref} assigneed to ${mr_assignee}\n\t\tTitle: ${mr_title}\n\t\tSource branch: ${mr_source_branch}\n\t\tLink: ${mr_link}"
        [ $DEBUG -eq 1 ] && (echo "${mr}" | jq )

        if [[ "${MERGE}" == 1 ]];then
            group=${mr_ref%/*}
            project_and_mr=${mr_ref#*/}
            project=${project_and_mr%!*}

            # Add remote, reset beta branch if necessary and fetch source branch
            get_add_fetch_remote "${project}" "${mr_source_project_id}" "${mr_source_branch}" "\t\t"

            echo -e "\t\tMerging ${CURRENT_REMOTE}/${mr_source_branch} to ${project}"
            cd "${CURDIR}/${project}"
            git merge "${CURRENT_REMOTE}/${mr_source_branch}" --no-squash | sed "s/^/\t\t/"
        fi
    done <<< "$MERGE_REQUESTS"
done <<< "$ISSUES"

# Change subprojects versions

if [[ "${MERGE}" == 1 ]];then

    echo -e "\n\n\e[32mChanging subprojects revisions\e[0m in packages of ${project}"
    get_add_fetch_remote recalbox 2396494 beta "\t"

    if [[ -f "recalbox-emulationstation/.stamp_reseted" ]];then
        cd "${CURDIR}/recalbox-emulationstation"
        commit_sha1=$(git rev-parse HEAD)
        cd "${CURDIR}/recalbox"
        echo -e "\tChanging RECALBOX_EMULATIONSTATION2_VERSION to ${commit_sha1}"
        sed -i "s/^RECALBOX_EMULATIONSTATION2_VERSION = .*/RECALBOX_EMULATIONSTATION2_VERSION = ${commit_sha1}/" package/recalbox-emulationstation2/recalbox-emulationstation2.mk
    fi
    if [[ -f "recalbox-configgen/.stamp_reseted" ]];then
        cd "${CURDIR}/recalbox-configgen"
        commit_sha1=$(git rev-parse HEAD)
        cd "${CURDIR}/recalbox"
        echo -e "\tChanging RECALBOX_CONFIGGEN_VERSION to ${commit_sha1}"
        sed -i "s/^RECALBOX_CONFIGGEN_VERSION = .*/RECALBOX_CONFIGGEN_VERSION = ${commit_sha1}/" package/recalbox-configgen/recalbox-configgen.mk
    fi
    if [[ -f "recalbox-themes-prime/.stamp_reseted" ]];then
        cd "${CURDIR}/recalbox-themes-prime"
        commit_sha1=$(git rev-parse HEAD)
        cd "${CURDIR}/recalbox"
        echo -e "\tChanging RECALBOX_THEMES_VERSION to ${commit_sha1}"
        sed -i "s/^RECALBOX_THEMES_VERSION = .*/RECALBOX_THEMES_VERSION = ${commit_sha1}/" package/recalbox-themes-prime/recalbox-themes-prime.mk
    fi
    # Commit subprojects version
    cd "${CURDIR}/recalbox"
    if [[ $(git ls-files -m | wc -l) != "0" ]];then
        echo -e "\tCommit subprojects version in .mk files"
        git add package/recalbox-emulationstation2/recalbox-emulationstation2.mk package/recalbox-configgen/recalbox-configgen.mk | sed "s/^/\t/"
        git commit -m "ci(beta): bumping subprojects to last beta version" | sed "s/^/\t/"
    else
        echo -e "\tNo subprojects modifications to commit"
    fi

    # Push all beta branches
    echo "Push your beta branches manually"
fi
