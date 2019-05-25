import pygame
import time

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
        pygame.init()
        pygame.display.init()
        pygame.mouse.set_visible(False)
        self.display()

    def __del__(self):
        pygame.display.quit()
        pygame.quit()

    def display(self):
        # change resolution
        w, h = self.getResolution()
        screen = pygame.display.set_mode((w, h))

        self.displayBackground(screen, w, h)
        self.displayGameData(screen, w, h)

        pygame.display.update()
        pygame.display.update()

    def displayGameData(self, screen, w, h):
        # get game data
        systemName, gameName, gameImagePath = self.loadGameData()

        # display image
        image = self.loadImage(gameImagePath)
        if image is not None:
            # Stretch and keep aspect ratio
            iw = w / 3
            # ratio = iw / image.get_width()
            ih = (image.get_height() * iw ) / image.get_width()
            #print("W / H = {}/{}".format(w,h))
            #print("iW/iH = {}/{}".format(iw,ih))
            image = pygame.transform.smoothscale(image, (iw, ih))
            # Display centered
            x = (w - iw) / 2
            y = h / 2
            screen.blit(image, (x, y))

        # display text
        margin = h / 40
        height = h / 20
        font1 = pygame.font.Font(self.FONT_PATH, (height * 2) / 3)
        font2 = pygame.font.Font(self.FONT_PATH, height)
        self.displayText(screen, "Recalbox was playing...", h / 4, font1, (255,255,255))
        self.displayText(screen, gameName, (h / 4) + height + margin, font2, (255,255,196))
        self.displayText(screen, systemName, (h / 4) + (height + margin) * 2, font1, (255,255,128))

    def displayText(self, screen, text, yoffset, font, color):
        textRendering = font.render(text, 1, color)
        screen.blit(textRendering, ((screen.get_width() - textRendering.get_width()) / 2, yoffset))

    def displayBackground(self, screen, w, h):
        # display background
        image = self.loadImage("/recalbox/system/resources/splash/demo.png")
        if image is not None:
            image = pygame.transform.smoothscale(image, (w, h))
            screen.blit(image, (0, 0))
        else:
            screen.fill((32, 32, 32))

    def getResolution(self):
        # get real screen info
        infoObject = pygame.display.Info()
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
            return pygame.image.load(path)
        except pygame.error:
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
    time.sleep(5)
    del demo