#!/bin/ash

# Show a picture. Pass Picture path as argument
display() {
  killall fbv
  fbv -f -i "$1" &
}

myself=$(readlink -f "$0")
myselfFolder=$(dirname "$myself")

# Display install picture
case "$1" in
  initialize) display "$myselfFolder/installInitializing.png" ;;
  createshare) display "$myselfFolder/installCreatingShare.png" ;;
  createoverlay) display "$myselfFolder/installCreatingOverlay.png" ;;
  installgames) display "$myselfFolder/installGames.png" ;;
  installstuff) display "$myselfFolder/installStuff.png" ;;
  finalize) display "$myselfFolder/installFinalizing.png" ;;
  *) display "$myselfFolder/install.png" ;;
esac
