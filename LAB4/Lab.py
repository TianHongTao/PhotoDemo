from PyQt5.QtWidgets import *
import sys
from Lab4 import Ui_Form

if __name__ == '__main__':
    app = QApplication(sys.argv)
    tmp = QWidget()
    ui = Ui_Form()
    ui.setupUi(tmp)
    tmp.show()
    sys.exit(app.exec_())