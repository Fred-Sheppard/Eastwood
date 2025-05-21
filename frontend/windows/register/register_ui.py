# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'register.ui'
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

class Ui_Register(object):
    def setupUi(self, Register):
        if not Register.objectName():
            Register.setObjectName(u"Register")
        Register.resize(440, 650)
        Register.setMinimumSize(QSize(440, 650))
        Register.setStyleSheet(u"background-color: #f5f6fa;")
        self.verticalLayout = QVBoxLayout(Register)
        self.verticalLayout.setSpacing(22)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(50, 45, 50, 45)
        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)

        self.titleLabel = QLabel(Register)
        self.titleLabel.setObjectName(u"titleLabel")
        self.titleLabel.setStyleSheet(u"font-size: 40px; font-weight: bold; color: #2d3436; letter-spacing: 1.2px; padding-bottom: 10px;")
        self.titleLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.titleLabel.setMinimumHeight(55)

        self.verticalLayout.addWidget(self.titleLabel)

        self.subtitleLabel = QLabel(Register)
        self.subtitleLabel.setObjectName(u"subtitleLabel")
        self.subtitleLabel.setStyleSheet(u"font-size: 14px; color: #636e72; margin-top: 0px; margin-bottom: 10px;")
        self.subtitleLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout.addWidget(self.subtitleLabel)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_2)

        self.fullNameEdit = QLineEdit(Register)
        self.fullNameEdit.setObjectName(u"fullNameEdit")
        self.fullNameEdit.setMinimumSize(QSize(0, 52))
        self.fullNameEdit.setStyleSheet(u"QLineEdit {\n"
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

        self.verticalLayout.addWidget(self.fullNameEdit)

        self.usernameEdit = QLineEdit(Register)
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

        self.passwordEdit = QLineEdit(Register)
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

        self.confirmPasswordEdit = QLineEdit(Register)
        self.confirmPasswordEdit.setObjectName(u"confirmPasswordEdit")
        self.confirmPasswordEdit.setMinimumSize(QSize(0, 52))
        self.confirmPasswordEdit.setStyleSheet(u"QLineEdit {\n"
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
        self.confirmPasswordEdit.setEchoMode(QLineEdit.EchoMode.Password)

        self.verticalLayout.addWidget(self.confirmPasswordEdit)

        self.passwordRequirementsLabel = QLabel(Register)
        self.passwordRequirementsLabel.setObjectName(u"passwordRequirementsLabel")
        self.passwordRequirementsLabel.setStyleSheet(u"color: #636e72; font-size: 10px; margin-left: 5px; margin-top: 1px;")
        self.passwordRequirementsLabel.setWordWrap(True)

        self.verticalLayout.addWidget(self.passwordRequirementsLabel)

        self.verticalSpacer_3 = QSpacerItem(20, 25, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_3)

        self.registerButton = QPushButton(Register)
        self.registerButton.setObjectName(u"registerButton")
        self.registerButton.setMinimumSize(QSize(0, 54))
        self.registerButton.setStyleSheet(u"QPushButton {\n"
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

        self.verticalLayout.addWidget(self.registerButton)

        self.passwordWarningLabel = QLabel(Register)
        self.passwordWarningLabel.setObjectName(u"passwordWarningLabel")
        self.passwordWarningLabel.setStyleSheet(u"color:rgb(198, 36, 18); font-size: 10px; margin-left: 5px; margin-top: 3px; font-weight: bold; letter-spacing: 0.3px;")
        self.passwordWarningLabel.setWordWrap(True)
        self.passwordWarningLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.verticalLayout.addWidget(self.passwordWarningLabel)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setSpacing(15)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(-1, 5, -1, 5)
        self.line1 = QFrame(Register)
        self.line1.setObjectName(u"line1")
        self.line1.setStyleSheet(u"background-color: #dfe6e9;")
        self.line1.setFrameShape(QFrame.Shape.HLine)
        self.line1.setFrameShadow(QFrame.Shadow.Sunken)

        self.horizontalLayout.addWidget(self.line1)

        self.orLabel = QLabel(Register)
        self.orLabel.setObjectName(u"orLabel")
        self.orLabel.setStyleSheet(u"color: #636e72; font-size: 13px;")
        self.orLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.horizontalLayout.addWidget(self.orLabel)

        self.line2 = QFrame(Register)
        self.line2.setObjectName(u"line2")
        self.line2.setStyleSheet(u"background-color: #dfe6e9;")
        self.line2.setFrameShape(QFrame.Shape.HLine)
        self.line2.setFrameShadow(QFrame.Shadow.Sunken)

        self.horizontalLayout.addWidget(self.line2)


        self.verticalLayout.addLayout(self.horizontalLayout)

        self.loginButton = QPushButton(Register)
        self.loginButton.setObjectName(u"loginButton")
        self.loginButton.setMinimumSize(QSize(0, 54))
        self.loginButton.setStyleSheet(u"QPushButton {\n"
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

        self.verticalLayout.addWidget(self.loginButton)

        self.verticalSpacer_4 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_4)


        self.retranslateUi(Register)

        QMetaObject.connectSlotsByName(Register)
    # setupUi

    def retranslateUi(self, Register):
        Register.setWindowTitle(QCoreApplication.translate("Register", u"Nightwood - Register", None))
        self.titleLabel.setText(QCoreApplication.translate("Register", u"Nightwood", None))
        self.subtitleLabel.setText(QCoreApplication.translate("Register", u"Create a new account to get started", None))
        self.fullNameEdit.setPlaceholderText(QCoreApplication.translate("Register", u"Full Name", None))
        self.usernameEdit.setPlaceholderText(QCoreApplication.translate("Register", u"Username", None))
        self.passwordEdit.setPlaceholderText(QCoreApplication.translate("Register", u"Password", None))
        self.confirmPasswordEdit.setPlaceholderText(QCoreApplication.translate("Register", u"Confirm Password", None))
        self.passwordRequirementsLabel.setText(QCoreApplication.translate("Register", u"Password must be at least 8 characters long. All characters are allowed, including spaces.", None))
        self.registerButton.setText(QCoreApplication.translate("Register", u"Create Account", None))
        self.passwordWarningLabel.setText(QCoreApplication.translate("Register", u"Ensure you remember your password\n"
"It cannot be recovered", None))
        self.orLabel.setText(QCoreApplication.translate("Register", u"or", None))
        self.loginButton.setText(QCoreApplication.translate("Register", u"Already have an account? Login", None))
    # retranslateUi

