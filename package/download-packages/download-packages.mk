################################################################################
#
# download-packages
#
################################################################################

DOWNLOAD_PACKAGES_LICENSE = RECALBOX

geturilastitem = $(lastword $(subst /, ,$(call qstrip,$(1))))

# gitlab(user,package,version): returns site of GitLab repository
gitlab = https://gitlab.com/$(1)/$(2)/-/archive/$(3)
