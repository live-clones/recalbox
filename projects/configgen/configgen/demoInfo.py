from settings.keyValueSettings import keyValueSettings

class demoInformation:

    # Information game from ES
    GAME_INFO_PATH = "/tmp/es_state.inf"

    # Key in informatiojn files
    KEY_SYSTEM_NAME = "System"
    KEY_GAME_NAME = "Game"
    KEY_GAME_IMAGE_PATH = "ImagePath"

    # font
    FONT_PATH = "/recalbox/share_init/system/.emulationstation/themes/recalbox-next/data/arts/ubuntu_bold.ttf"

    # Forced resolution - use 0,0 to use current screen resolution
    FORCE_WIDTH = 0
    FORCE_HEIGHT = 0

    def __init__(self):
        self.time = __import__("time")
        self.pygame = __import__("pygame")
        os = __import__("os")
        if not os.path.exists("/dev/input/mice"):
            os.environ["SDL_NOMOUSE"] = "1"
        self.pygame.init()
        self.pygame.display.init()
        self.pygame.mouse.set_visible(False)

        # Prepare Background
        backgroundImage = self.loadImage("/recalbox/system/resources/splash/demo.png")
        w, h = self.getResolution()
        self.backgroundImage = self.prepareBackground(backgroundImage, w, h)

        # get game data
        self.systemName, self.gameName, gameImagePath = self.loadGameData()

        # Prepare game image
        self.gameImage = self.prepareGameData(gameImagePath, h)

        # Prepare font
        self.fontMargin = h / 40
        self.fontHeight = h / 20
        self.font1 = self.pygame.font.Font(self.FONT_PATH, (self.fontHeight * 2) / 3)
        self.font2 = self.pygame.font.Font(self.FONT_PATH, self.fontHeight)

    def __del__(self):
        self.pygame.display.quit()
        self.pygame.quit()

    def display(self):
        # change resolution
        w, h = self.getResolution()
        screen = self.pygame.display.set_mode((w, h))

        self.displayBackground(screen)
        self.displayGameData(screen, w, h)

        self.pygame.display.update()
        self.pygame.display.update()

    def prepareGameData(self, gameImagePath, h):
        # display image
        image = self.loadImage(gameImagePath)
        if image is not None:
            # Stretch and keep aspect ratio
            ih = (h * 9) / 20
            iw = (image.get_width() * ih ) / image.get_height()
            #print("W / H = {}/{}".format(w,h))
            #print("iW/iH = {}/{}".format(iw,ih))
            return self.pygame.transform.smoothscale(image, (iw, ih))

        return None

    def displayGameData(self, screen, w, h):

        # display image
        if self.gameImage is not None:
            # Display centered
            x = (w - self.gameImage.get_width()) / 2
            y = h / 2
            screen.blit(self.gameImage, (x, y))

        # display text
        self.displayText(screen, "Recalbox was playing...", h / 4, self.font1, (255,255,255))
        self.displayText(screen, self.gameName, (h / 4) + self.fontHeight + self.fontMargin, self.font2, (255,255,196))
        self.displayText(screen, self.systemName, (h / 4) + (self.fontHeight + self.fontMargin) * 2, self.font1, (255,255,128))

    @staticmethod
    def displayText(screen, text, yoffset, font, color):
        textRendering = font.render(text, 1, color)
        screen.blit(textRendering, ((screen.get_width() - textRendering.get_width()) / 2, yoffset))

    def prepareBackground(self, image, w, h):
        # display background
        if image is not None:
            return self.pygame.transform.smoothscale(image, (w, h))
        return None

    def displayBackground(self, screen):
        # display background
        if self.backgroundImage is not None:
            screen.blit(self.backgroundImage, (0, 0))
        else:
            screen.fill((32, 32, 32))

    def getResolution(self):
        # get real screen info
        infoObject = self.pygame.display.Info()
        sw = infoObject.current_w
        sh = infoObject.current_h

        # get wanted resolution
        w = self.FORCE_WIDTH
        h = self.FORCE_HEIGHT

        # auto adjust
        if w * h == 0:
            if w != 0 and h == 0:
                h = (w * sh) / sw
            elif w == 0 and h != 0:
                w = (h * sw) / sh
            else:
                w = sw
                h = sh

        return w, h

    def loadImage(self, path):
        try:
            return self.pygame.image.load(path)
        except self.pygame.error:
            return None

    def loadGameData(self):
        gameInfo = keyValueSettings(self.GAME_INFO_PATH, False)
        gameInfo.loadFile(True)

        systemName = gameInfo.getOption(self.KEY_SYSTEM_NAME, "Unknown")
        gameName = gameInfo.getOption(self.KEY_GAME_NAME, "Unknown")
        gameImagePath = gameInfo.getOption(self.KEY_GAME_IMAGE_PATH, "")
        del gameInfo

        return systemName, gameName, gameImagePath

if __name__ == '__main__':
    demo = demoInformation()
    __import__("time").sleep(5)
    del demo