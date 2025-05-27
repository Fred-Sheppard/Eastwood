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
from PySide6.QtWidgets import (QApplication, QHBoxLayout, QLabel, QLineEdit,
    QPushButton, QSizePolicy, QSpacerItem, QVBoxLayout,
    QWidget)
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
        self.navBar = QWidget(Settings)
        self.navBar.setObjectName(u"navBar")
        self.navBar.setMinimumSize(QSize(220, 0))
        self.navBar.setMaximumSize(QSize(220, 16777215))
        self.navBar.setStyleSheet(u"QWidget#navBar {\n"
"  background-color: white;\n"
"  border-right: 1px solid #dfe6e9;\n"
"}")
        self.navBarLayout = QVBoxLayout(self.navBar)
        self.navBarLayout.setSpacing(30)
        self.navBarLayout.setObjectName(u"navBarLayout")
        self.navBarLayout.setContentsMargins(25, 30, 25, 30)
        self.logoLayout = QHBoxLayout()
        self.logoLayout.setSpacing(12)
        self.logoLayout.setObjectName(u"logoLayout")
        self.logoLabel = QLabel(self.navBar)
        self.logoLabel.setObjectName(u"logoLabel")
        self.logoLabel.setMinimumSize(QSize(32, 32))
        self.logoLabel.setMaximumSize(QSize(32, 32))
        self.logoLabel.setStyleSheet(u"background-color: #6c5ce7;\n"
"border-radius: 6px;")

        self.logoLayout.addWidget(self.logoLabel)

        self.logoTextLabel = QLabel(self.navBar)
        self.logoTextLabel.setObjectName(u"logoTextLabel")
        self.logoTextLabel.setStyleSheet(u"font-size: 20px; font-weight: bold; color: #2d3436; letter-spacing: 0.5px;")

        self.logoLayout.addWidget(self.logoTextLabel)


        self.navBarLayout.addLayout(self.logoLayout)

        self.receivedButton = QPushButton(self.navBar)
        self.receivedButton.setObjectName(u"receivedButton")
        self.receivedButton.setMinimumSize(QSize(0, 42))
        self.receivedButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  color: #2d3436;\n"
"  background-color: transparent;\n"
"  border-radius: 8px;\n"
"  text-align: left;\n"
"  padding-left: 15px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #f1f2f6;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #dfe6e9;\n"
"}")

        self.navBarLayout.addWidget(self.receivedButton)

        self.sentButton = QPushButton(self.navBar)
        self.sentButton.setObjectName(u"sentButton")
        self.sentButton.setMinimumSize(QSize(0, 42))
        self.sentButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  color: #2d3436;\n"
"  background-color: transparent;\n"
"  border-radius: 8px;\n"
"  text-align: left;\n"
"  padding-left: 15px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #f1f2f6;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #dfe6e9;\n"
"}")

        self.navBarLayout.addWidget(self.sentButton)

        self.sendFileButton = QPushButton(self.navBar)
        self.sendFileButton.setObjectName(u"sendFileButton")
        self.sendFileButton.setMinimumSize(QSize(0, 42))
        self.sendFileButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  color: #2d3436;\n"
"  background-color: transparent;\n"
"  border-radius: 8px;\n"
"  text-align: left;\n"
"  padding-left: 15px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #f1f2f6;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #dfe6e9;\n"
"}")

        self.navBarLayout.addWidget(self.sendFileButton)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.navBarLayout.addItem(self.verticalSpacer)

        self.settingsButton = QPushButton(self.navBar)
        self.settingsButton.setObjectName(u"settingsButton")
        self.settingsButton.setMinimumSize(QSize(0, 42))
        self.settingsButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  font-weight: 500;\n"
"  color: white;\n"
"  background-color: #6c5ce7;\n"
"  border-radius: 8px;\n"
"  text-align: left;\n"
"  padding-left: 15px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #5049c9;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #4040b0;\n"
"}")

        self.navBarLayout.addWidget(self.settingsButton)


        self.mainLayout.addWidget(self.navBar)

        self.contentLayout = QVBoxLayout()
        self.contentLayout.setSpacing(20)
        self.contentLayout.setObjectName(u"contentLayout")
        self.contentLayout.setContentsMargins(35, 35, 35, 35)
        self.pageTitle = QLabel(Settings)
        self.pageTitle.setObjectName(u"pageTitle")
        self.pageTitle.setStyleSheet(u"font-size: 24px; font-weight: bold; color: #2d3436; margin-bottom: 10px;")

        self.contentLayout.addWidget(self.pageTitle)

        self.topSpacer = QSpacerItem(20, 20, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.contentLayout.addItem(self.topSpacer)

        self.passwordWidget = QWidget(Settings)
        self.passwordWidget.setObjectName(u"passwordWidget")
        self.passwordWidget.setMinimumSize(QSize(0, 400))
        self.passwordWidget.setStyleSheet(u"QWidget#passwordWidget {\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 12px;\n"
"  padding: 30px;\n"
"}")
        self.passwordLayout = QVBoxLayout(self.passwordWidget)
        self.passwordLayout.setSpacing(20)
        self.passwordLayout.setObjectName(u"passwordLayout")
        self.passwordLayout.setContentsMargins(20, -1, 20, -1)
        self.passwordSectionTitle = QLabel(self.passwordWidget)
        self.passwordSectionTitle.setObjectName(u"passwordSectionTitle")
        self.passwordSectionTitle.setStyleSheet(u"font-size: 18px; font-weight: bold; color: #2d3436; margin-bottom: 10px;")

        self.passwordLayout.addWidget(self.passwordSectionTitle)

        self.currentPasswordLabel = QLabel(self.passwordWidget)
        self.currentPasswordLabel.setObjectName(u"currentPasswordLabel")
        self.currentPasswordLabel.setStyleSheet(u"font-size: 14px; color: #2d3436; font-weight: 500; margin-bottom: 5px;")

        self.passwordLayout.addWidget(self.currentPasswordLabel)

        self.currentPassword = QLineEdit(self.passwordWidget)
        self.currentPassword.setObjectName(u"currentPassword")
        self.currentPassword.setMinimumSize(QSize(0, 40))
        self.currentPassword.setEchoMode(QLineEdit.Password)
        self.currentPassword.setStyleSheet(u"QLineEdit {\n"
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

        self.passwordLayout.addWidget(self.currentPassword)

        self.newPasswordLabel = QLabel(self.passwordWidget)
        self.newPasswordLabel.setObjectName(u"newPasswordLabel")
        self.newPasswordLabel.setStyleSheet(u"font-size: 14px; color: #2d3436; font-weight: 500; margin-bottom: 5px;")

        self.passwordLayout.addWidget(self.newPasswordLabel)

        self.newPassword = QLineEdit(self.passwordWidget)
        self.newPassword.setObjectName(u"newPassword")
        self.newPassword.setMinimumSize(QSize(0, 40))
        self.newPassword.setEchoMode(QLineEdit.Password)
        self.newPassword.setStyleSheet(u"QLineEdit {\n"
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

        self.passwordLayout.addWidget(self.newPassword)

        self.confirmPasswordLabel = QLabel(self.passwordWidget)
        self.confirmPasswordLabel.setObjectName(u"confirmPasswordLabel")
        self.confirmPasswordLabel.setStyleSheet(u"font-size: 14px; color: #2d3436; font-weight: 500; margin-bottom: 5px;")

        self.passwordLayout.addWidget(self.confirmPasswordLabel)

        self.confirmPassword = QLineEdit(self.passwordWidget)
        self.confirmPassword.setObjectName(u"confirmPassword")
        self.confirmPassword.setMinimumSize(QSize(0, 40))
        self.confirmPassword.setEchoMode(QLineEdit.Password)
        self.confirmPassword.setStyleSheet(u"QLineEdit {\n"
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

        self.passwordLayout.addWidget(self.confirmPassword)

        self.passwordRequirements = QLabel(self.passwordWidget)
        self.passwordRequirements.setObjectName(u"passwordRequirements")
        self.passwordRequirements.setStyleSheet(u"font-size: 12px; color: #636e72; margin-top: 5px;")

        self.passwordLayout.addWidget(self.passwordRequirements)


        self.contentLayout.addWidget(self.passwordWidget)

        self.bottomSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.contentLayout.addItem(self.bottomSpacer)

        self.buttonLayout = QHBoxLayout()
        self.buttonLayout.setObjectName(u"buttonLayout")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.buttonLayout.addItem(self.horizontalSpacer)

        self.cancelButton = QPushButton(Settings)
        self.cancelButton.setObjectName(u"cancelButton")
        self.cancelButton.setMinimumSize(QSize(130, 44))
        self.cancelButton.setStyleSheet(u"QPushButton {\n"
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

        self.buttonLayout.addWidget(self.cancelButton)

        self.saveButton = QPushButton(Settings)
        self.saveButton.setObjectName(u"saveButton")
        self.saveButton.setMinimumSize(QSize(130, 44))
        self.saveButton.setStyleSheet(u"QPushButton {\n"
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

        self.buttonLayout.addWidget(self.saveButton)


        self.contentLayout.addLayout(self.buttonLayout)


        self.mainLayout.addLayout(self.contentLayout)


        self.retranslateUi(Settings)

        QMetaObject.connectSlotsByName(Settings)
    # setupUi

    def retranslateUi(self, Settings):
        Settings.setWindowTitle(QCoreApplication.translate("Settings", u"Nightwood - Settings", None))
        self.logoLabel.setText("")
        self.logoTextLabel.setText(QCoreApplication.translate("Settings", u"Nightwood", None))
        self.receivedButton.setText(QCoreApplication.translate("Settings", u"Received Files", None))
        self.sentButton.setText(QCoreApplication.translate("Settings", u"Sent Files", None))
        self.sendFileButton.setText(QCoreApplication.translate("Settings", u"Send File", None))
        self.settingsButton.setText(QCoreApplication.translate("Settings", u"Settings", None))
        self.pageTitle.setText(QCoreApplication.translate("Settings", u"Settings", None))
        self.passwordSectionTitle.setText(QCoreApplication.translate("Settings", u"Change Password", None))
        self.currentPasswordLabel.setText(QCoreApplication.translate("Settings", u"Current Password", None))
        self.newPasswordLabel.setText(QCoreApplication.translate("Settings", u"New Password", None))
        self.confirmPasswordLabel.setText(QCoreApplication.translate("Settings", u"Confirm New Password", None))
        self.passwordRequirements.setText(QCoreApplication.translate("Settings", u"Password must be between 8 and 64 characters", None))
        self.cancelButton.setText(QCoreApplication.translate("Settings", u"Cancel", None))
        self.saveButton.setText(QCoreApplication.translate("Settings", u"Save Changes", None))
    # retranslateUi

