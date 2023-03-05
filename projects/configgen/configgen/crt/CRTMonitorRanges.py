from typing import Dict

from configgen.crt.CRTTypes import CRTScreenType
from configgen.crt.MonitorRange import MonitorRange

CRTMonitorRanges: Dict[CRTScreenType, MonitorRange] = {
    CRTScreenType.k15: MonitorRange(15625, 15750, 49.50, 65.00, 2.000, 4.700, 8.000, 0.064, 0.192, 1.024, 0, 0, 192,
                                    288, 448, 576),
    CRTScreenType.k15Ext: MonitorRange(15500, 15750, 49.50, 65.00, 2.000, 4.700, 8.000, 0.064, 0.192, 1.024, 0, 0, 192,
                                       288, 448, 576),
    CRTScreenType.k31: MonitorRange(31400, 31600, 100, 130, 0.671, 2.683, 3.353, 0.034, 0.101, 0.436, 0, 0, 200,
                                    256, 0, 0)
}
