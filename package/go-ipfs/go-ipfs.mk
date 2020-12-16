################################################################################
#
# go-ipfs
#
################################################################################

GO_IPFS_VERSION = 0.7.0
GO_IPFS_SITE = https://github.com/ipfs/go-ipfs/releases/download/v$(GO_IPFS_VERSION)
GO_IPFS_SOURCE = go-ipfs-source.tar.gz

GO_IPFS_LICENSE = MIT or Apache-2.0
GO_IPFS_LICENSE_FILES = LICENSE LICENSE-MIT LICENSE-APACHE

GO_IPFS_BUILD_TARGETS = cmd/ipfs

GO_IPFS_DEPENDENCIES = openssl
GO_IPFS_TAGS = openssl
GO_IPFS_LDFLAGS = \
	-X $(GO_IPFS_GOMOD)/cli/version.GitCommit=$(GO_IPFS_VERSION) \
	-X $(GO_IPFS_GOMOD)/cli/version.Version=$(GO_IPFS_VERSION) \
	-linkmode internal

GO_IPFS_INSTALL_BINS = $(notdir $(GO_IPFS_BUILD_TARGETS))

define GO_IPFS_INSTALL_INIT_SYSV
	$(INSTALL) -D -m 0755 $(GO_IPFS_PKGDIR)/S10ipfs $(TARGET_DIR)/etc/init.d/S10ipfs
endef

$(eval $(golang-package))
