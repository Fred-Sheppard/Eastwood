# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'navbar.ui'
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
from PySide6.QtWidgets import (QApplication, QHBoxLayout, QLabel, QPushButton,
    QSizePolicy, QSpacerItem, QVBoxLayout, QWidget)

class Ui_NavBar(object):
    def setupUi(self, NavBar):
        if not NavBar.objectName():
            NavBar.setObjectName(u"NavBar")
        NavBar.setMinimumSize(QSize(200, 0))
        NavBar.setMaximumSize(QSize(200, 16777215))
        NavBar.setStyleSheet(u"QWidget#NavBar {\n"
"  background-color: white;\n"
"  border-right: 1px solid #e9ecef;\n"
"}")
        self.navBarLayout = QVBoxLayout(NavBar)
        self.navBarLayout.setSpacing(17)
        self.navBarLayout.setObjectName(u"navBarLayout")
        self.navBarLayout.setContentsMargins(20, 28, 20, 28)
        self.logoLayout = QHBoxLayout()
        self.logoLayout.setSpacing(12)
        self.logoLayout.setObjectName(u"logoLayout")
        self.logoLabel = QLabel(NavBar)
        self.logoLabel.setObjectName(u"logoLabel")
        self.logoLabel.setMinimumSize(QSize(30, 30))
        self.logoLabel.setMaximumSize(QSize(30, 30))
        self.logoLabel.setStyleSheet(u"background-color: #6c5ce7;\n"
"border-radius: 6px;")

        self.logoLayout.addWidget(self.logoLabel)

        self.logoTextLabel = QLabel(NavBar)
        self.logoTextLabel.setObjectName(u"logoTextLabel")
        self.logoTextLabel.setStyleSheet(u"font-size: 18px; font-weight: 600; color: #2d3436; letter-spacing: 0.3px;")

        self.logoLayout.addWidget(self.logoTextLabel)


        self.navBarLayout.addLayout(self.logoLayout)

        self.receivedButton = QPushButton(NavBar)
        self.receivedButton.setObjectName(u"receivedButton")
        self.receivedButton.setMinimumSize(QSize(0, 40))
        self.receivedButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 13px;\n"
"  font-weight: 500;\n"
"  color: white;\n"
"  background-color: #6c5ce7;\n"
"  border-radius: 6px;\n"
"  text-align: left;\n"
"  padding-left: 14px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #5049c9;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #4040b0;\n"
"}")

        self.navBarLayout.addWidget(self.receivedButton)

        self.sentButton = QPushButton(NavBar)
        self.sentButton.setObjectName(u"sentButton")
        self.sentButton.setMinimumSize(QSize(0, 40))
        self.sentButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 13px;\n"
"  color: #2d3436;\n"
"  background-color: transparent;\n"
"  border-radius: 6px;\n"
"  text-align: left;\n"
"  padding-left: 14px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #f8f9fa;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #e9ecef;\n"
"}")

        self.navBarLayout.addWidget(self.sentButton)

        self.sendFileButton = QPushButton(NavBar)
        self.sendFileButton.setObjectName(u"sendFileButton")
        self.sendFileButton.setMinimumSize(QSize(0, 40))
        self.sendFileButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 13px;\n"
"  color: #2d3436;\n"
"  background-color: transparent;\n"
"  border-radius: 6px;\n"
"  text-align: left;\n"
"  padding-left: 14px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #f8f9fa;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #e9ecef;\n"
"}")

        self.navBarLayout.addWidget(self.sendFileButton)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.navBarLayout.addItem(self.verticalSpacer)

        self.settingsButton = QPushButton(NavBar)
        self.settingsButton.setObjectName(u"settingsButton")
        self.settingsButton.setMinimumSize(QSize(0, 40))
        self.settingsButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 13px;\n"
"  color: #2d3436;\n"
"  background-color: transparent;\n"
"  border-radius: 6px;\n"
"  text-align: left;\n"
"  padding-left: 14px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #f8f9fa;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #e9ecef;\n"
"}")

        self.navBarLayout.addWidget(self.settingsButton)

        self.logoutButton = QPushButton(NavBar)
        self.logoutButton.setObjectName(u"logoutButton")
        self.logoutButton.setMinimumSize(QSize(0, 40))
        self.logoutButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 13px;\n"
"  color: #e74c3c;\n"
"  background-color: transparent;\n"
"  border-radius: 6px;\n"
"  text-align: left;\n"
"  padding-left: 14px;\n"
"  border: 1px solid #e74c3c;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #fff5f5;\n"
"  border-color: #c0392b;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #ffe3e3;\n"
"  border-color: #a93226;\n"
"}")

        self.navBarLayout.addWidget(self.logoutButton)


        self.retranslateUi(NavBar)

        QMetaObject.connectSlotsByName(NavBar)
    # setupUi

    def retranslateUi(self, NavBar):
        self.logoLabel.setText("")
        self.logoTextLabel.setText(QCoreApplication.translate("NavBar", u"Nightwood", None))
        self.receivedButton.setText(QCoreApplication.translate("NavBar", u"Received Files", None))
        self.sentButton.setText(QCoreApplication.translate("NavBar", u"Sent Files", None))
        self.sendFileButton.setText(QCoreApplication.translate("NavBar", u"Send File", None))
        self.settingsButton.setText(QCoreApplication.translate("NavBar", u"Settings", None))
        self.logoutButton.setText(QCoreApplication.translate("NavBar", u"Logout", None))
        pass
    # retranslateUi

