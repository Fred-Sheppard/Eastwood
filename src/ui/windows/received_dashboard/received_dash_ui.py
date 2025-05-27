# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'received_dash.ui'
##
## Created by: Qt User Interface Compiler version 6.9.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QAbstractItemView, QApplication, QHBoxLayout, QLabel,
    QListWidget, QListWidgetItem, QPushButton, QSizePolicy,
    QSpacerItem, QVBoxLayout, QWidget)

from src.ui.utils.navbar.navbar import NavBar

class Ui_Received(object):
    def setupUi(self, Received):
        if not Received.objectName():
            Received.setObjectName(u"Received")
        Received.resize(1000, 700)
        Received.setMinimumSize(QSize(1000, 700))
        Received.setStyleSheet(u"background-color: white;")
        self.mainLayout = QHBoxLayout(Received)
        self.mainLayout.setSpacing(0)
        self.mainLayout.setObjectName(u"mainLayout")
        self.mainLayout.setContentsMargins(0, 0, 0, 0)
        self.navBar = NavBar(Received)
        self.navBar.setObjectName(u"navBar")
        self.navBar.setMinimumSize(QSize(220, 0))
        self.navBar.setMaximumSize(QSize(220, 16777215))

        self.mainLayout.addWidget(self.navBar)

        self.contentLayout = QVBoxLayout()
        self.contentLayout.setSpacing(25)
        self.contentLayout.setObjectName(u"contentLayout")
        self.contentLayout.setContentsMargins(35, 35, 35, 35)
        self.pageTitle = QLabel(Received)
        self.pageTitle.setObjectName(u"pageTitle")
        self.pageTitle.setStyleSheet(u"font-size: 24px; font-weight: bold; color: #2d3436;")

        self.contentLayout.addWidget(self.pageTitle)

        self.fileList = QListWidget(Received)
        self.fileList.setObjectName(u"fileList")
        self.fileList.setStyleSheet(u"QListWidget {\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 12px;\n"
"  padding: 5px;\n"
"}\n"
"QListWidget::item {\n"
"  background-color: white;\n"
"  border-bottom: 1px solid #f1f2f6;\n"
"}\n"
"QListWidget::item:last-child {\n"
"  border-bottom: none;\n"
"}\n"
"QListWidget::item:hover {\n"
"  background-color: #f8f9fa;\n"
"}\n"
"QScrollBar:vertical {\n"
"  border: none;\n"
"  background: #f5f6fa;\n"
"  width: 8px;\n"
"  margin: 0;\n"
"}\n"
"QScrollBar::handle:vertical {\n"
"  background: #dfe6e9;\n"
"  border-radius: 4px;\n"
"  min-height: 30px;\n"
"}\n"
"QScrollBar::handle:vertical:hover {\n"
"  background: #b2bec3;\n"
"}\n"
"QScrollBar::add-line:vertical,\n"
"QScrollBar::sub-line:vertical {\n"
"  height: 0px;\n"
"}\n"
"QScrollBar::add-page:vertical,\n"
"QScrollBar::sub-page:vertical {\n"
"  background: none;\n"
"}")
        self.fileList.setVerticalScrollMode(QAbstractItemView.ScrollPerPixel)
        self.fileList.setHorizontalScrollMode(QAbstractItemView.ScrollPerPixel)

        self.contentLayout.addWidget(self.fileList)

        self.bottomLayout = QHBoxLayout()
        self.bottomLayout.setObjectName(u"bottomLayout")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.bottomLayout.addItem(self.horizontalSpacer)

        self.sendButton = QPushButton(Received)
        self.sendButton.setObjectName(u"sendButton")
        self.sendButton.setMinimumSize(QSize(130, 44))
        self.sendButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  font-weight: bold;\n"
"  color: white;\n"
"  background-color: #6c5ce7;\n"
"  border-radius: 10px;\n"
"  padding: 0 20px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #5049c9;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #4040b0;\n"
"}")

        self.bottomLayout.addWidget(self.sendButton)


        self.contentLayout.addLayout(self.bottomLayout)


        self.mainLayout.addLayout(self.contentLayout)


        self.retranslateUi(Received)

        QMetaObject.connectSlotsByName(Received)
    # setupUi

    def retranslateUi(self, Received):
        Received.setWindowTitle(QCoreApplication.translate("Received", u"Nightwood - Received Files", None))
        self.pageTitle.setText(QCoreApplication.translate("Received", u"Received Files", None))
        self.sendButton.setText(QCoreApplication.translate("Received", u"Send File", None))
    # retranslateUi

