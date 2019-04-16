# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Lab4.ui'
#
# Created by: PyQt5 UI code generator 5.9.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5.QtCore import pyqtSlot
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import *
from PyQt5.QtGui import QIcon,QPixmap,QFont,QImage
import cv2
import numpy as np

class Ui_Form(object):
    def setupUi(self, Form):
        self.THRESHOLD_AREA = 20000
        Form.setObjectName("Form")
        Form.resize(788, 505)
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout(Form)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.verticalLayout = QtWidgets.QVBoxLayout()
        self.verticalLayout.setObjectName("verticalLayout")
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label = QtWidgets.QLabel(Form)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.label_2 = QtWidgets.QLabel(Form)
        self.label_2.setObjectName("label_2")
        self.horizontalLayout.addWidget(self.label_2)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.chooseButton = QtWidgets.QPushButton(Form)
        self.chooseButton.setObjectName("chooseButton")
        self.chooseButton.clicked.connect(self.clickChooseButton)
        self.pushButton = QtWidgets.QPushButton(Form)
        self.pushButton.setObjectName("pushButton")
        self.pushButton.clicked.connect(self.clickPushButton)
        self.verticalLayout.addWidget(self.chooseButton)
        self.verticalLayout.addWidget(self.pushButton)
        self.horizontalLayout_2.addLayout(self.verticalLayout)
        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "水瓶装满检测器"))
        self.label.setText(_translate("Form", "检测处理前"))
        self.label_2.setText(_translate("Form", "检测处理后"))
        self.chooseButton.setText(_translate("Form", "选择图片"))
        self.pushButton.setText(_translate("Form", "检测"))

    @pyqtSlot()
    def clickChooseButton(self):
        fileDialog = QFileDialog()
        fname, _ = QFileDialog.getOpenFileName(fileDialog, 'OpenFile', "/Users/denhiroshi/PycharmProjects/Lab4",
                                               "Image files (*.jpg *.bmp *.png)")
        self.img = cv2.imread(fname, cv2.IMREAD_COLOR)
        self.label.setPixmap(QPixmap(fname))

    @pyqtSlot()
    def clickPushButton(self):
        img = self.img
        # 转为灰度图
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        # 高斯滤波
        gaussianImg = cv2.GaussianBlur(gray, (5, 5), 1.5)
        # 开运算
        openImg = cv2.morphologyEx(gaussianImg, cv2.MORPH_OPEN, (5, 5))
        # 二值化处理
        ret, thresh = cv2.threshold(openImg, 190, 255, cv2.THRESH_TOZERO)
        # 边沿检测
        _, contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        # 创建一个新的三通道图（和原图的大小一样）
        b = np.zeros((img.shape[0], img.shape[1]), dtype=img.dtype)
        g = np.zeros((img.shape[0], img.shape[1]), dtype=img.dtype)
        r = np.zeros((img.shape[0], img.shape[1]), dtype=img.dtype)
        threshNew = cv2.merge([b, g, r])

        # 在原图中轮廓大小大于
        for i in range(len(contours)):
            if cv2.contourArea(contours[i]) > self.THRESHOLD_AREA:
                cv2.drawContours(img, contours, i, (255, 0, 0), -1)

        height, width, channel = img.shape
        bytesPerLine = 3 * width
        qImg = QImage(img.data, width, height, bytesPerLine, QImage.Format_RGB888)
        self.label_2.setPixmap(QPixmap(qImg))


