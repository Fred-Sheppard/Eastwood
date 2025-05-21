# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'login.ui'
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

class Ui_Login(object):
    def setupUi(self, Login):
        if not Login.objectName():
            Login.setObjectName(u"Login")
        Login.resize(440, 600)
        Login.setMinimumSize(QSize(440, 600))
        Login.setStyleSheet(u"background-color: #f5f6fa;")
        self.verticalLayout = QVBoxLayout(Login)
        self.verticalLayout.setSpacing(22)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(50, 45, 50, 45)
        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)

        self.titleLabel = QLabel(Login)
        self.titleLabel.setObjectName(u"titleLabel")
        self.titleLabel.setStyleSheet(u"font-size: 40px; font-weight: bold; color: #2d3436; letter-spacing: 1.2px; margin-bottom: 5px;")
        self.titleLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout.addWidget(self.titleLabel)

        self.subtitleLabel = QLabel(Login)
        self.subtitleLabel.setObjectName(u"subtitleLabel")
        self.subtitleLabel.setStyleSheet(u"font-size: 14px; color: #636e72; margin-top: 0px; margin-bottom: 10px;")
        self.subtitleLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout.addWidget(self.subtitleLabel)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_2)

        self.usernameEdit = QLineEdit(Login)
        self.usernameEdit.setObjectName(u"usernameEdit")
        self.usernameEdit.setMinimumSize(QSize(0, 52))
        self.usernameEdit.setStyleSheet(u"QLineEdit {\n"
"  padding-left: 15px;\n"
"  font-size: 15px;\n"
"  border-radius: 10px;\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  color: #2d3436;\n"
"}\n"
"QLineEdit:focus {\n"
"  border: 2px solid #6c5ce7;\n"
"}")

        self.verticalLayout.addWidget(self.usernameEdit)

        self.passwordEdit = QLineEdit(Login)
        self.passwordEdit.setObjectName(u"passwordEdit")
        self.passwordEdit.setMinimumSize(QSize(0, 52))
        self.passwordEdit.setStyleSheet(u"QLineEdit {\n"
"  padding-left: 15px;\n"
"  font-size: 15px;\n"
"  border-radius: 10px;\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  color: #2d3436;\n"
"}\n"
"QLineEdit:focus {\n"
"  border: 2px solid #6c5ce7;\n"
"}")
        self.passwordEdit.setEchoMode(QLineEdit.EchoMode.Password)

        self.verticalLayout.addWidget(self.passwordEdit)

        self.forgotPasswordLayout = QHBoxLayout()
        self.forgotPasswordLayout.setObjectName(u"forgotPasswordLayout")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.forgotPasswordLayout.addItem(self.horizontalSpacer)


        self.verticalLayout.addLayout(self.forgotPasswordLayout)

        self.verticalSpacer_3 = QSpacerItem(20, 25, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_3)

        self.loginButton = QPushButton(Login)
        self.loginButton.setObjectName(u"loginButton")
        self.loginButton.setMinimumSize(QSize(0, 54))
        self.loginButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 16px;\n"
"  font-weight: bold;\n"
"  color: white;\n"
"  background-color: #6c5ce7;\n"
"  border-radius: 10px;\n"
"  border: none;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #5049c9;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #4040b0;\n"
"}")

        self.verticalLayout.addWidget(self.loginButton)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setSpacing(15)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(-1, 5, -1, 5)
        self.line1 = QFrame(Login)
        self.line1.setObjectName(u"line1")
        self.line1.setStyleSheet(u"background-color: #dfe6e9;")
        self.line1.setFrameShape(QFrame.Shape.HLine)
        self.line1.setFrameShadow(QFrame.Shadow.Sunken)

        self.horizontalLayout.addWidget(self.line1)

        self.orLabel = QLabel(Login)
        self.orLabel.setObjectName(u"orLabel")
        self.orLabel.setStyleSheet(u"color: #636e72; font-size: 13px;")
        self.orLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout.addWidget(self.orLabel)

        self.line2 = QFrame(Login)
        self.line2.setObjectName(u"line2")
        self.line2.setStyleSheet(u"background-color: #dfe6e9;")
        self.line2.setFrameShape(QFrame.Shape.HLine)
        self.line2.setFrameShadow(QFrame.Shadow.Sunken)

        self.horizontalLayout.addWidget(self.line2)


        self.verticalLayout.addLayout(self.horizontalLayout)

        self.registerButton = QPushButton(Login)
        self.registerButton.setObjectName(u"registerButton")
        self.registerButton.setMinimumSize(QSize(0, 54))
        self.registerButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 16px;\n"
"  font-weight: bold;\n"
"  border-radius: 10px;\n"
"  color: #6c5ce7;\n"
"  background-color: white;\n"
"  border: 2px solid #6c5ce7;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #f5f3ff;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #eeeaff;\n"
"}")

        self.verticalLayout.addWidget(self.registerButton)

        self.verticalSpacer_4 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_4)


        self.retranslateUi(Login)

        QMetaObject.connectSlotsByName(Login)
    # setupUi

    def retranslateUi(self, Login):
        Login.setWindowTitle(QCoreApplication.translate("Login", u"Nightwood - Login", None))
        self.titleLabel.setText(QCoreApplication.translate("Login", u"NightWood", None))
        self.subtitleLabel.setText(QCoreApplication.translate("Login", u"Welcome back! Please login to continue", None))
        self.usernameEdit.setPlaceholderText(QCoreApplication.translate("Login", u"Username", None))
        self.passwordEdit.setPlaceholderText(QCoreApplication.translate("Login", u"Password", None))
        self.loginButton.setText(QCoreApplication.translate("Login", u"Login", None))
        self.orLabel.setText(QCoreApplication.translate("Login", u"or", None))
        self.registerButton.setText(QCoreApplication.translate("Login", u"Create Account", None))
    # retranslateUi

