#!/bin/bash
set -euo pipefail

# Be sure to run the project in an empty directory for the first time.

## TODO: Add pipeline state

if [ -z "${1:-}" ]; then
    echo "Usage : bash beta_merges.sh GITLAB_TOKEN TAG [MERGE] [COMMENT_ON_MR] [PUSH] [DEBUG]
    Show info: bash beta_merges.sh XXXXX '7.3-beta6'
    Show infos and merge : bash beta_merges.sh XXXXX '7.3-beta6' 1
    Show infos and merge and comment and push : bash beta_merges.sh XXXXX '7.3-beta6' 1 1 1
    Show infos and merge and comment and push and debug : bash beta_merges.sh XXXXX '7.3-beta6' 1 1 1 1" && exit 1
fi

TOKEN="${1}"
TAG="${2:-}"
MERGE="${3:-0}"
COMMENT="${4:-0}"
PUSH="${5:-0}"
DEBUG="${6:-0}"
MR_SKIP=""

if [[ "${DEBUG}" == "1" ]];then set -x;fi

CURDIR=$(pwd)

rm -f */.stamp_reseted
rm -f .tag_message.md

function tagmessage() {
    echo -e "${1}" >> "${CURDIR}/.tag_message.md"
}

tagmessage "# Beta ${TAG}\n

### List of embedded Merge Requests:

"

function get_add_fetch_remote() {
    local project="${1}"
    local source_project_id="${2}"
    local source_branch="${3}"
    local tabs="${4}"
    local project_info=$(curl -sf --header "PRIVATE-TOKEN: ${TOKEN}" "https://gitlab.com/api/v4/projects/${source_project_id}" | jq)

    local ssh_url_to_repo=$( echo "${project_info}" | jq -r ".ssh_url_to_repo" )
    local namespace=$( echo "${project_info}" | jq -r ".namespace.full_path" )

    cd "${CURDIR}"
    if [ ! -d "${CURDIR}/${project}" ];then
        echo -e "${tabs}\e[0mCloning project git@gitlab.com:recalbox/${project}.git to ${project}\e[2m"
        git clone "git@gitlab.com:recalbox/${project}.git" 2>&1 | sed "s/^/${tabs}/"
        cd "${project}"
        (git remote add recalbox "git@gitlab.com:recalbox/${project}.git" 2>&1 | sed "s/^/${tabs}/") || true
    fi
    cd "${CURDIR}/${project}"
    if [[ ! $(git remote get-url "${namespace}") ]];then
        echo -e "${tabs}\e[0mAdding remote ${namespace} on project ${project}\e[2m"
        git remote add "${namespace}" "${ssh_url_to_repo}" | sed "s/^/${tabs}/"
    fi
    git fetch -p "${namespace}" | sed "s/^/${tabs}/"

    if [ ! -f "${CURDIR}/${project}/.stamp_reseted" ];then
        echo -e "${tabs}\e[0mReseting beta branches on project ${project}\e[2m"
        git fetch recalbox -p 2>&1 | sed "s/^/${tabs}/"
        git reset --hard 2>&1 | sed "s/^/${tabs}/"
        (git checkout -b beta recalbox/beta > /dev/null 2>&1 ) || (git checkout -b beta > /dev/null 2>&1 | sed "s/^/${tabs}/") || (git checkout beta > /dev/null 2>&1 | sed "s/^/${tabs}/")
        if git branch | grep -q "master"; then
            git reset --hard recalbox/master 2>&1 | sed "s/^/${tabs}/"
        else
            git reset --hard recalbox/main 2>&1 | sed "s/^/${tabs}/"
        fi
        echo "1" > "${CURDIR}/${project}/.stamp_reseted"
    fi

    CURRENT_REMOTE="${namespace}"
    cd "${CURDIR}"
}



echo -e "\n--------------------------------------------
Checking all merge with label Testing::Beta
--------------------------------------------"

MERGE_REQUESTS=$(curl -sf --header "PRIVATE-TOKEN: ${TOKEN}" "https://gitlab.com/api/v4/projects/2396494/merge_requests?labels=Testing::Beta&state=opened&per_page=100" | jq -c '.[]')

    
if [ $? != 0 ]; then 
    echo "Unable to fetch merge requests, exiting"
    exit 1
fi
[ $DEBUG -eq 1 ] && echo "${MERGE_REQUESTS}"

readarray mrArray < <(echo "${MERGE_REQUESTS}")

for mr in "${mrArray[@]}";do
    MR_COMMENT=":rocket: Merged in [${TAG}](https://gitlab.com/recalbox/recalbox/-/tags/${TAG}):"

    project_id=$( echo "${mr}" | jq ".project_id" )
    # Don't merge already merged issues
    mr_state=$( echo "${mr}" | jq -r ".state" )
    if [[ "${mr_state}" != "opened" ]]; then
        continue
    fi
    mr_title=$( echo "${mr}" | jq -r ".title" )
    mr_iid=$( echo "${mr}" | jq -r ".iid" )
    mr_ref=$( echo "${mr}" | jq -r ".references.full" )
    if [[ "${mr_ref}" == "${MR_SKIP}" ]];then
        continue
    fi
    mr_relative_ref=$( echo "${mr}" | jq -r ".references.relative" )
    mr_assignee=$( echo "${mr}" | jq -r ".assignee.username" )
    if [[ "${mr_assignee}" == "null" ]];then mr_assignee="";fi

    mr_link=$( echo "${mr}" | jq -r ".web_url" )
    mr_sha=$( echo "${mr}" | jq -r ".sha" )
    mr_source_branch=$( echo "${mr}" | jq -r ".source_branch" )
    mr_source_project_id=$( echo "${mr}" | jq -r ".source_project_id" )
    echo -e "\t\e[34mMerge request\e[0m ${mr_ref} assigneed to ${mr_assignee}\n\t\tTitle: ${mr_title}\n\t\tLink: ${mr_link}"
    tagmessage "  - **Merge request**: ${mr_title} [(${mr_ref})](${mr_link}) ${mr_assignee:+"assigneed to @${mr_assignee}"}"

    [ $DEBUG -eq 1 ] && (echo "${mr}" | jq )


    group=${mr_ref%/*}
    project_and_mr=${mr_ref#*/}
    project=${project_and_mr%!*}

    MR_COMMENT=":rocket: Merged in [${TAG}](https://gitlab.com/recalbox/recalbox/-/tags/${TAG}): https://gitlab.com/${group}/${project}/-/commit/${mr_sha}"

    if [[ "${MERGE}" == 1 ]];then

        # Add remote, reset beta branch if necessary and fetch source branch
        get_add_fetch_remote "${project}" "${mr_source_project_id}" "${mr_source_branch}" "\t\t"

        echo -e "\t\tMerging ${CURRENT_REMOTE}/${mr_source_branch} to ${project}"
        cd "${CURDIR}/${project}"
        #git merge "${CURRENT_REMOTE}/${mr_source_branch}" --no-squash | sed "s/^/\t\t/"
        echo -e "\t\t\t\e[0m- Deleting temp branch\e[2m"
        (git branch -D temp 2>&1 || true) | sed "s/^/\t\t\t/"
        
        echo -e "\t\t\t\e[0m- Checking out merge request branch on temp branch\e[2m"
        git checkout -b temp "${CURRENT_REMOTE}/${mr_source_branch}" 2>&1 | sed "s/^/\t\t\t/"

        echo -e "\t\t\t\e[0m- Rebasing on beta\e[2m"
        set +e
        git rebase "beta" 2>&1 | sed "s/^/\t\t\t/"
        if [[ "$?" != "0" ]];then
            echo -e "\t\t\t\e[0m\e[31m- Please resolve conflicts on temp branch of project ${project}\e[0m\n\t\t\tPress ENTER to continue..."
            read a
        fi
        set -e
        echo -e "\t\t\t\e[0m- Checking out beta\e[2m"
        git checkout beta 2>&1 | sed "s/^/\t\t\t/"

        echo -e "\t\t\t\e[0m- Merging temp branch on beta\e[2m"
        git merge temp 2>&1 | sed "s/^/\t\t\t/"
    fi

    if [[ "${COMMENT}" == 1 ]];then
        echo -e "Merge Request Comment: ${MR_COMMENT}" | sed "s/^/\t/"
        COMMENT_RESPONSE=$(curl -sf --header "PRIVATE-TOKEN: ${TOKEN}" -X POST \
            "https://gitlab.com/api/v4/projects/2396494/merge_requests/${mr_iid}/notes" \
            --data-urlencod "body=${MR_COMMENT}" | jq -c '.[]')
    fi
done

# Change subprojects versions

if [[ "${MERGE}" == 1 ]];then

    # Commit subprojects version
    cd "${CURDIR}/recalbox"

    # Push all beta branches
    if [[ "${PUSH}" == "1" ]];then
        echo -e "\n\n\e[32mPushing your beta branches\e[0m"
        cd "${CURDIR}/recalbox"
        git push recalbox beta:beta -f 2>&1 | sed "s/^/\t/"

        tagmessage "\n### TESTING.md\n"
        git diff master...beta --no-ext-diff --unified=0 -a --no-prefix -- TESTING.md | egrep "^\+-" | sed "s/^+//" >> "${CURDIR}/.tag_message.md"
        
        cd "${CURDIR}"
        if [[ "${TAG}" != "" ]];then
            # Create tag
            echo -e "\n\n\e[32mCreating TAG\e[0m ${TAG}"
            TAG_RESPONSE=$(curl -sf --header "PRIVATE-TOKEN: ${TOKEN}" -X POST \
            "https://gitlab.com/api/v4/projects/2396494/repository/tags/?tag_name=${TAG}&ref=beta" | jq -c '.[]')

            echo -e "\n\n\e[32mCreating RELEASE\e[0m ${TAG}"

            RELEASE_RESPONSE=$(curl -sf --header "PRIVATE-TOKEN: ${TOKEN}" -X POST \
            --data-urlencode "description=$(cat "${CURDIR}/.tag_message.md")" \
            -d "name=${TAG}" \
            -d "tag_name=${TAG}" \
            "https://gitlab.com/api/v4/projects/2396494/releases/" | jq -c '.[]')
        fi
    fi
fi
