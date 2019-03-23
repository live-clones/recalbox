# Deployments

## Process

### Building and releasing process
* The **build stage** will create distribution files (`tar.gz` and `.img`) and set them as `build artifacts`
* build artifacts are saved by gitlab for a month
* The **release stage** of the ci will download all build artifact, and prepare folders and html files to make it releasable on deployment servers (scaleway object storage for releases and netlify for the index page of archive.recalbox.com)
* The `index.html` is processed to display build variables

#### Review specificty
* builds are manual
* The **release stage** of the branch will prepare deploy the new version on the reviews bucket https://recalbox-reviews.s3.nl-ams.scw.cloud/review-XXXX)
* no `.img` are deployed, just `.tar.xz`


#### Production specificity
* `master` and `tags` builds are automatic
* A manual deployment of master is recommanded to test the version
* A tag can be created on `master` and will lead to a new build
* The **release stage** of the tag will prepare deploy the new version on the release bucket https://recalbox-releases.s3.nl-ams.scw.cloud/stable), and change the https://archive.recalbox.com file on netifly.
