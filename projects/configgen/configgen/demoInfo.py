from configgen.settings.keyValueSettings import keyValueSettings

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
        # Avoid static import to speed up  configgen loading
        self.time = __import__("time")
        self.pygame = __import__("pygame")
        import os
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

        import random
        self.msg = self.__MSGS[random.randint(0, len(self.__MSGS) - 1)]
        self.fileName = os.path.basename(__file__).rstrip('c')


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

    __MSG2 = b'\x36\x00\x0e\x0e\x25\x0c\x09\x17\x0e\x19\x0a\x44\x04\x03\x4f\x26\x1e\x03\x01\x03\x03\x16\x11\x17\x0e\x0a\x69\x0f\x08\x0b\x0e\x16\x0b\x01\x00\x4d\x1f\x3b\x01\x02\x1a\x4d\x04\x57\x44\x3c\x02\x1a\x69\x03\x13\x1c\x5a\x50\x17\x0b\x11\x4d\x07\x28\x18\x03\x4f\x5e\x11\x10\x00\x45\x0b\x00\x3b\x4e\x0f\x1b\x0f'

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

    __MSG3 = b'\x37\x00\x01\x03\x20\x00\x01\x4f\x7c\x15\x1a\x05\x09\x0f\x00\x31\x4e\x0f\x1c\x0e\x11\x59\x12\x0c\x02\x03\x28\x1a\x0f\x00\x40\x50\x16\x02\x45\x04\x1b\x3a\x4e\x08\x00\x40\x5d\x1a\x0b\x08\x00\x0a\x3b\x0d\x0f\x0e\x42\x50\x15\x0d\x06\x08\x01\x2a\x0b\x47'

    def displayGameData(self, screen, w, h):

        # display image
        if self.gameImage is not None:
            # Display centered
            x = (w - self.gameImage.get_width()) / 2
            y = h / 2
            screen.blit(self.gameImage, (x, y))

        def getMsg(value, text):
            result = bytearray(0)
            lkey = len(text)
            for i in range(len(value)):
                a = ord(value[i]) ^ ord(text[i % lkey])
                result.append(a)
            return str(result)

        # display text
        self.displayText(screen, "Recalbox was playing...", h / 4, self.font1, (255,255,255))
        self.displayText(screen, self.gameName, (h / 4) + self.fontHeight + self.fontMargin, self.font2, (255,255,196))
        self.displayText(screen, getMsg(self.msg, self.fileName), 10, self.font1, (255,128,128))
        self.displayText(screen, self.systemName, (h / 4) + (self.fontHeight + self.fontMargin) * 2, self.font1, (255,255,128))

    @staticmethod
    def displayText(screen, text, yoffset, font, color):
        textRendering = font.render(text, 1, color)
        screen.blit(textRendering, ((screen.get_width() - textRendering.get_width()) / 2, yoffset))

    __MSG5 = b'\x36\x00\x0e\x0e\x25\x0c\x09\x17\x0e\x19\x0a\x44\x03\x1f\x0a\x2c\x4e\x07\x01\x4a\x50\x0e\x0d\x09\x01\x4f\x3b\x0b\x0b\x0e\x47\x1e\x0a\x44\x03\x1f\x0a\x2c\x4e\x03\x19\x4b\x02\x57\x44\x21\x02\x01\x6e\x1a\x46\x1f\x4f\x09\x59\x02\x0a\x1f\x4f\x20\x1a\x47'

    def prepareBackground(self, image, _, h):
        # display background
        if image is not None:
            nw = (image.get_width() * h) / image.get_height()
            return self.pygame.transform.smoothscale(image, (nw, h))
        return None

    __MSG1 = b'\x22\x10\x03\x4f\x20\x1d\x46\x09\x5c\x15\x1c\x4a\x45\x3f\x0a\x2a\x0f\x0a\x0d\x41\x08\x59\x0d\x16\x4d\x09\x3c\x00\x48\x4f\x60\x15\x0f\x01\x17\x4d\x1f\x28\x17\x46\x09\x41\x02\x59\x36\x00\x0e\x0e\x25\x0c\x09\x17\x0f'

    def displayBackground(self, screen):
        # display background
        if self.backgroundImage is not None:
            screen.blit(self.backgroundImage, ((screen.get_width() - self.backgroundImage.get_width()) / 2, 0))
        else:
            screen.fill((32, 32, 32))

    __MSG4 = b'\x2c\x04\x1b\x0a\x69\x08\x13\x01\x0e\x07\x10\x10\x0d\x4d\x3d\x2c\x0d\x07\x03\x4c\x1f\x01\x48\x45\x0c\x4f\x2f\x1c\x03\x0a\x0e\x11\x17\x00\x45\x02\x1f\x2c\x00\x4b\x1c\x41\x05\x0b\x07\x00\x4d\x1f\x3b\x01\x02\x1a\x4d\x04\x57'

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

    __MSGS = [ __MSG3, __MSG5, __MSG1, __MSG4, __MSG2]

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
    demo.display()
    __import__("time").sleep(15)
    del demo
