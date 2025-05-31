# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'settings.ui'
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
from PySide6.QtWidgets import (QApplication, QFrame, QHBoxLayout, QLabel,
    QLineEdit, QPushButton, QSizePolicy, QSpacerItem,
    QVBoxLayout, QWidget)

from src.ui.utils.navbar.navbar import NavBar

class Ui_Settings(object):
    def setupUi(self, Settings):
        if not Settings.objectName():
            Settings.setObjectName(u"Settings")
        Settings.resize(1000, 700)
        Settings.setMinimumSize(QSize(1000, 700))
        Settings.setStyleSheet(u"background-color: white;")
        self.mainLayout = QHBoxLayout(Settings)
        self.mainLayout.setSpacing(0)
        self.mainLayout.setObjectName(u"mainLayout")
        self.mainLayout.setContentsMargins(0, 0, 0, 0)
        self.navBar = NavBar(Settings)
        self.navBar.setObjectName(u"navBar")
        self.navBar.setMinimumSize(QSize(220, 0))
        self.navBar.setMaximumSize(QSize(220, 16777215))

        self.mainLayout.addWidget(self.navBar)

        self.contentLayout = QVBoxLayout()
        self.contentLayout.setSpacing(20)
        self.contentLayout.setObjectName(u"contentLayout")
        self.contentLayout.setContentsMargins(35, 35, 35, 35)
        self.topSpacer = QSpacerItem(20, 20, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.contentLayout.addItem(self.topSpacer)

        self.pageTitle = QLabel(Settings)
        self.pageTitle.setObjectName(u"pageTitle")
        self.pageTitle.setStyleSheet(u"font-size: 24px; font-weight: bold; color: #2d3436; margin-bottom: 10px;")

        self.contentLayout.addWidget(self.pageTitle)

        self.settingsContentLayout = QHBoxLayout()
        self.settingsContentLayout.setSpacing(30)
        self.settingsContentLayout.setObjectName(u"settingsContentLayout")
        self.passphraseWidget = QWidget(Settings)
        self.passphraseWidget.setObjectName(u"passphraseWidget")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(1)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.passphraseWidget.sizePolicy().hasHeightForWidth())
        self.passphraseWidget.setSizePolicy(sizePolicy)
        self.passphraseWidget.setMinimumSize(QSize(0, 400))
        self.passphraseWidget.setStyleSheet(u"QWidget#passphraseWidget {\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 12px;\n"
"  padding: 30px;\n"
"}")
        self.passphraseLayout = QVBoxLayout(self.passphraseWidget)
        self.passphraseLayout.setSpacing(20)
        self.passphraseLayout.setObjectName(u"passphraseLayout")
        self.passphraseLayout.setContentsMargins(20, -1, 20, -1)
        self.passphraseSectionTitle = QLabel(self.passphraseWidget)
        self.passphraseSectionTitle.setObjectName(u"passphraseSectionTitle")
        self.passphraseSectionTitle.setStyleSheet(u"font-size: 18px; font-weight: bold; color: #2d3436; margin-bottom: 10px;")

        self.passphraseLayout.addWidget(self.passphraseSectionTitle)

        self.currentPassphraseLabel = QLabel(self.passphraseWidget)
        self.currentPassphraseLabel.setObjectName(u"currentPassphraseLabel")
        self.currentPassphraseLabel.setStyleSheet(u"font-size: 14px; color: #2d3436; font-weight: 500; margin-bottom: 5px;")

        self.passphraseLayout.addWidget(self.currentPassphraseLabel)

        self.currentPassphrase = QLineEdit(self.passphraseWidget)
        self.currentPassphrase.setObjectName(u"currentPassphrase")
        self.currentPassphrase.setMinimumSize(QSize(0, 40))
        self.currentPassphrase.setStyleSheet(u"QLineEdit {\n"
"  padding: 8px 12px;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 6px;\n"
"  background-color: #f8f9fa;\n"
"  color: #2d3436;\n"
"  font-size: 14px;\n"
"}\n"
"QLineEdit:focus {\n"
"  border: 1px solid #6c5ce7;\n"
"  background-color: white;\n"
"}")
        self.currentPassphrase.setEchoMode(QLineEdit.EchoMode.Password)

        self.passphraseLayout.addWidget(self.currentPassphrase)

        self.newPassphraseLabel = QLabel(self.passphraseWidget)
        self.newPassphraseLabel.setObjectName(u"newPassphraseLabel")
        self.newPassphraseLabel.setStyleSheet(u"font-size: 14px; color: #2d3436; font-weight: 500; margin-bottom: 5px;")

        self.passphraseLayout.addWidget(self.newPassphraseLabel)

        self.newPassphrase = QLineEdit(self.passphraseWidget)
        self.newPassphrase.setObjectName(u"newPassphrase")
        self.newPassphrase.setMinimumSize(QSize(0, 40))
        self.newPassphrase.setStyleSheet(u"QLineEdit {\n"
"  padding: 8px 12px;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 6px;\n"
"  background-color: #f8f9fa;\n"
"  color: #2d3436;\n"
"  font-size: 14px;\n"
"}\n"
"QLineEdit:focus {\n"
"  border: 1px solid #6c5ce7;\n"
"  background-color: white;\n"
"}")
        self.newPassphrase.setEchoMode(QLineEdit.EchoMode.Password)

        self.passphraseLayout.addWidget(self.newPassphrase)

        self.confirmPassphraseLabel = QLabel(self.passphraseWidget)
        self.confirmPassphraseLabel.setObjectName(u"confirmPassphraseLabel")
        self.confirmPassphraseLabel.setStyleSheet(u"font-size: 14px; color: #2d3436; font-weight: 500; margin-bottom: 5px;")

        self.passphraseLayout.addWidget(self.confirmPassphraseLabel)

        self.confirmPassphrase = QLineEdit(self.passphraseWidget)
        self.confirmPassphrase.setObjectName(u"confirmPassphrase")
        self.confirmPassphrase.setMinimumSize(QSize(0, 40))
        self.confirmPassphrase.setStyleSheet(u"QLineEdit {\n"
"  padding: 8px 12px;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 6px;\n"
"  background-color: #f8f9fa;\n"
"  color: #2d3436;\n"
"  font-size: 14px;\n"
"}\n"
"QLineEdit:focus {\n"
"  border: 1px solid #6c5ce7;\n"
"  background-color: white;\n"
"}")
        self.confirmPassphrase.setEchoMode(QLineEdit.EchoMode.Password)

        self.passphraseLayout.addWidget(self.confirmPassphrase)

        self.passphraseRequirements = QLabel(self.passphraseWidget)
        self.passphraseRequirements.setObjectName(u"passphraseRequirements")
        self.passphraseRequirements.setStyleSheet(u"font-size: 12px; color: #636e72; margin-top: 5px;")
        self.passphraseRequirements.setWordWrap(True)

        self.passphraseLayout.addWidget(self.passphraseRequirements)

        self.passphraseSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.passphraseLayout.addItem(self.passphraseSpacer)

        self.passphraseButtonLayout = QHBoxLayout()
        self.passphraseButtonLayout.setObjectName(u"passphraseButtonLayout")
        self.passphraseHorizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.passphraseButtonLayout.addItem(self.passphraseHorizontalSpacer)

        self.passphraseCancelButton = QPushButton(self.passphraseWidget)
        self.passphraseCancelButton.setObjectName(u"passphraseCancelButton")
        self.passphraseCancelButton.setMinimumSize(QSize(130, 44))
        self.passphraseCancelButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  font-weight: bold;\n"
"  color: #2d3436;\n"
"  background-color: #dfe6e9;\n"
"  border-radius: 10px;\n"
"  padding: 0 20px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #b2bec3;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #a4b0be;\n"
"}")

        self.passphraseButtonLayout.addWidget(self.passphraseCancelButton)

        self.passphraseSaveButton = QPushButton(self.passphraseWidget)
        self.passphraseSaveButton.setObjectName(u"passphraseSaveButton")
        self.passphraseSaveButton.setMinimumSize(QSize(130, 44))
        self.passphraseSaveButton.setStyleSheet(u"QPushButton {\n"
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

        self.passphraseButtonLayout.addWidget(self.passphraseSaveButton)


        self.passphraseLayout.addLayout(self.passphraseButtonLayout)


        self.settingsContentLayout.addWidget(self.passphraseWidget)

        self.verticalLine = QFrame(Settings)
        self.verticalLine.setObjectName(u"verticalLine")
        self.verticalLine.setStyleSheet(u"background-color: #dfe6e9;")
        self.verticalLine.setFrameShape(QFrame.Shape.VLine)
        self.verticalLine.setFrameShadow(QFrame.Shadow.Sunken)

        self.settingsContentLayout.addWidget(self.verticalLine)

        self.authWidget = QWidget(Settings)
        self.authWidget.setObjectName(u"authWidget")
        sizePolicy.setHeightForWidth(self.authWidget.sizePolicy().hasHeightForWidth())
        self.authWidget.setSizePolicy(sizePolicy)
        self.authWidget.setMinimumSize(QSize(0, 400))
        self.authWidget.setStyleSheet(u"QWidget#authWidget {\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 12px;\n"
"  padding: 30px;\n"
"}")
        self.authLayout = QVBoxLayout(self.authWidget)
        self.authLayout.setSpacing(20)
        self.authLayout.setObjectName(u"authLayout")
        self.authLayout.setContentsMargins(20, -1, 20, -1)
        self.authSectionTitle = QLabel(self.authWidget)
        self.authSectionTitle.setObjectName(u"authSectionTitle")
        self.authSectionTitle.setStyleSheet(u"font-size: 18px; font-weight: bold; color: #2d3436; margin-bottom: 10px;")

        self.authLayout.addWidget(self.authSectionTitle)

        self.authDescription = QLabel(self.authWidget)
        self.authDescription.setObjectName(u"authDescription")
        self.authDescription.setStyleSheet(u"font-size: 14px; color: #636e72; margin-bottom: 20px;")
        self.authDescription.setWordWrap(True)

        self.authLayout.addWidget(self.authDescription)

        self.authCodeInput = QLineEdit(self.authWidget)
        self.authCodeInput.setObjectName(u"authCodeInput")
        self.authCodeInput.setMinimumSize(QSize(0, 40))
        self.authCodeInput.setStyleSheet(u"QLineEdit {\n"
"  padding: 8px 12px;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 6px;\n"
"  background-color: #f8f9fa;\n"
"  color: #2d3436;\n"
"  font-size: 14px;\n"
"}\n"
"QLineEdit:focus {\n"
"  border: 1px solid #6c5ce7;\n"
"  background-color: white;\n"
"}")

        self.authLayout.addWidget(self.authCodeInput)

        self.scanQRButton = QPushButton(self.authWidget)
        self.scanQRButton.setObjectName(u"scanQRButton")
        self.scanQRButton.setMinimumSize(QSize(0, 40))
        self.scanQRButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  font-weight: bold;\n"
"  color: #6c5ce7;\n"
"  background-color: white;\n"
"  border: 2px solid #6c5ce7;\n"
"  border-radius: 6px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #f5f3ff;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #eeeaff;\n"
"}")

        self.authLayout.addWidget(self.scanQRButton)

        self.qrCodeLabel = QLabel(self.authWidget)
        self.qrCodeLabel.setObjectName(u"qrCodeLabel")
        self.qrCodeLabel.setMinimumSize(QSize(200, 200))
        self.qrCodeLabel.setStyleSheet(u"background-color: #f8f9fa; border-radius: 6px;")
        self.qrCodeLabel.setAlignment(Qt.AlignCenter)

        self.authLayout.addWidget(self.qrCodeLabel)

        self.authSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.authLayout.addItem(self.authSpacer)

        self.authButtonLayout = QHBoxLayout()
        self.authButtonLayout.setObjectName(u"authButtonLayout")
        self.authHorizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.authButtonLayout.addItem(self.authHorizontalSpacer)

        self.authCancelButton = QPushButton(self.authWidget)
        self.authCancelButton.setObjectName(u"authCancelButton")
        self.authCancelButton.setMinimumSize(QSize(130, 44))
        self.authCancelButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  font-weight: bold;\n"
"  color: #2d3436;\n"
"  background-color: #dfe6e9;\n"
"  border-radius: 10px;\n"
"  padding: 0 20px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #b2bec3;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #a4b0be;\n"
"}")

        self.authButtonLayout.addWidget(self.authCancelButton)

        self.authSaveButton = QPushButton(self.authWidget)
        self.authSaveButton.setObjectName(u"authSaveButton")
        self.authSaveButton.setMinimumSize(QSize(130, 44))
        self.authSaveButton.setStyleSheet(u"QPushButton {\n"
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

        self.authButtonLayout.addWidget(self.authSaveButton)


        self.authLayout.addLayout(self.authButtonLayout)


        self.settingsContentLayout.addWidget(self.authWidget)


        self.contentLayout.addLayout(self.settingsContentLayout)

        self.bottomSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.contentLayout.addItem(self.bottomSpacer)


        self.mainLayout.addLayout(self.contentLayout)


        self.retranslateUi(Settings)

        QMetaObject.connectSlotsByName(Settings)
    # setupUi

    def retranslateUi(self, Settings):
        Settings.setWindowTitle(QCoreApplication.translate("Settings", u"Nightwood - Settings", None))
        self.pageTitle.setText(QCoreApplication.translate("Settings", u"Settings", None))
        self.passphraseSectionTitle.setText(QCoreApplication.translate("Settings", u"Change Passphrase", None))
        self.currentPassphraseLabel.setText(QCoreApplication.translate("Settings", u"Current Passphrase", None))
        self.newPassphraseLabel.setText(QCoreApplication.translate("Settings", u"New Passphrase", None))
        self.confirmPassphraseLabel.setText(QCoreApplication.translate("Settings", u"Confirm New Passphrase", None))
        self.passphraseRequirements.setText(QCoreApplication.translate("Settings", u"Passphrase must be between 20 and 64 characters", None))
        self.passphraseCancelButton.setText(QCoreApplication.translate("Settings", u"Cancel", None))
        self.passphraseSaveButton.setText(QCoreApplication.translate("Settings", u"Save", None))
        self.authSectionTitle.setText(QCoreApplication.translate("Settings", u"Authentication Code", None))
        self.authDescription.setText(QCoreApplication.translate("Settings", u"Enter your authentication code or scan the QR code to verify your device.", None))
        self.authCodeInput.setPlaceholderText(QCoreApplication.translate("Settings", u"Enter authentication code", None))
        self.scanQRButton.setText(QCoreApplication.translate("Settings", u"Scan QR Code", None))
        self.authCancelButton.setText(QCoreApplication.translate("Settings", u"Cancel", None))
        self.authSaveButton.setText(QCoreApplication.translate("Settings", u"Verify", None))
    # retranslateUi

