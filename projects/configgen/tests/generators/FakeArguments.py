

class Arguments:

    def __init__(self, rom, *, verbose=False):
        self.rom = rom
        self.nodefaultkeymap = False
        self.demo = False
        self.verbose = verbose
        self.system = 'virtual'
        self.test = False
        self.disabledsoftpatching = False
        self.ips = ''
        self.bps = ''
        self.ups = ''
