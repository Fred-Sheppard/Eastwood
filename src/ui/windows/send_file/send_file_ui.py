# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'send_file.ui'
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

class Ui_SendFile(object):
    def setupUi(self, SendFile):
        if not SendFile.objectName():
            SendFile.setObjectName(u"SendFile")
        SendFile.resize(1000, 700)
        SendFile.setMinimumSize(QSize(1000, 700))
        SendFile.setStyleSheet(u"background-color: white;")
        self.mainLayout = QHBoxLayout(SendFile)
        self.mainLayout.setSpacing(0)
        self.mainLayout.setObjectName(u"mainLayout")
        self.mainLayout.setContentsMargins(0, 0, 0, 0)
        self.navBar = QWidget(SendFile)
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

        self.navBarLayout.addWidget(self.sendFileButton)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.navBarLayout.addItem(self.verticalSpacer)

        self.settingsButton = QPushButton(self.navBar)
        self.settingsButton.setObjectName(u"settingsButton")
        self.settingsButton.setMinimumSize(QSize(0, 42))
        self.settingsButton.setStyleSheet(u"QPushButton {\n"
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

        self.navBarLayout.addWidget(self.settingsButton)


        self.mainLayout.addWidget(self.navBar)

        self.contentLayout = QVBoxLayout()
        self.contentLayout.setSpacing(25)
        self.contentLayout.setObjectName(u"contentLayout")
        self.contentLayout.setContentsMargins(35, 35, 35, 35)
        self.pageTitle = QLabel(SendFile)
        self.pageTitle.setObjectName(u"pageTitle")
        self.pageTitle.setStyleSheet(u"font-size: 24px; font-weight: bold; color: #2d3436;")

        self.contentLayout.addWidget(self.pageTitle)

        self.formContainer = QWidget(SendFile)
        self.formContainer.setObjectName(u"formContainer")
        self.formContainer.setStyleSheet(u"QWidget#formContainer {\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 12px;\n"
"  padding: 25px;\n"
"}\n"
"QLineEdit {\n"
"  padding: 8px 12px;\n"
"  font-size: 14px;\n"
"  border-radius: 6px;\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  color: #2d3436;\n"
"  margin: 4px 0;\n"
"  min-height: 20px;\n"
"}\n"
"QLineEdit:focus {\n"
"  border: 2px solid #6c5ce7;\n"
"}\n"
"QLabel {\n"
"  color: #2d3436;\n"
"  font-weight: bold;\n"
"  font-size: 14px;\n"
"  margin: 8px 0 4px 0;\n"
"}\n"
"QPushButton#browseButton {\n"
"  color: #2d3436;\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 6px;\n"
"  min-height: 32px;\n"
"  max-height: 32px;\n"
"  font-size: 13px;\n"
"  padding: 4px 12px;\n"
"}\n"
"QPushButton#browseButton:hover {\n"
"  background-color: #f1f2f6;\n"
"  border-color: #6c5ce7;\n"
"  color: #6c5ce7;\n"
"}\n"
"QLabel#fileDetailsLabel {\n"
"  font-size: 13px;\n"
"  font-weight: normal;\n"
"  color: #636e72;\n"
" "
                        " padding: 12px;\n"
"  background-color: #f8f9fa;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 6px;\n"
"  margin: 8px 0;\n"
"  line-height: 1.4;\n"
"}")
        self.formLayout = QVBoxLayout(self.formContainer)
        self.formLayout.setSpacing(15)
        self.formLayout.setObjectName(u"formLayout")
        self.formLayout.setContentsMargins(25, 25, 25, 25)
        self.usernameLabel = QLabel(self.formContainer)
        self.usernameLabel.setObjectName(u"usernameLabel")

        self.formLayout.addWidget(self.usernameLabel)

        self.usernameInput = QLineEdit(self.formContainer)
        self.usernameInput.setObjectName(u"usernameInput")
        self.usernameInput.setMinimumSize(QSize(0, 42))

        self.formLayout.addWidget(self.usernameInput)

        self.fileLabel = QLabel(self.formContainer)
        self.fileLabel.setObjectName(u"fileLabel")

        self.formLayout.addWidget(self.fileLabel)

        self.fileLayout = QHBoxLayout()
        self.fileLayout.setSpacing(8)
        self.fileLayout.setObjectName(u"fileLayout")
        self.filePathInput = QLineEdit(self.formContainer)
        self.filePathInput.setObjectName(u"filePathInput")
        self.filePathInput.setMinimumSize(QSize(0, 42))
        self.filePathInput.setReadOnly(True)

        self.fileLayout.addWidget(self.filePathInput)

        self.browseButton = QPushButton(self.formContainer)
        self.browseButton.setObjectName(u"browseButton")
        self.browseButton.setMinimumSize(QSize(90, 42))
        self.browseButton.setMaximumSize(QSize(90, 42))

        self.fileLayout.addWidget(self.browseButton)


        self.formLayout.addLayout(self.fileLayout)

        self.fileDetailsLabel = QLabel(self.formContainer)
        self.fileDetailsLabel.setObjectName(u"fileDetailsLabel")
        self.fileDetailsLabel.setMinimumSize(QSize(0, 120))
        self.fileDetailsLabel.setWordWrap(True)

        self.formLayout.addWidget(self.fileDetailsLabel)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.formLayout.addItem(self.verticalSpacer_2)

        self.sendButton = QPushButton(self.formContainer)
        self.sendButton.setObjectName(u"sendButton")
        self.sendButton.setMinimumSize(QSize(130, 44))
        self.sendButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  font-weight: bold;\n"
"  color: white;\n"
"  background-color: #6c5ce7;\n"
"  border-radius: 10px;\n"
"  padding: 0 10px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #5049c9;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #4040b0;\n"
"}")

        self.formLayout.addWidget(self.sendButton, 0, Qt.AlignHCenter)


        self.contentLayout.addWidget(self.formContainer)


        self.mainLayout.addLayout(self.contentLayout)


        self.retranslateUi(SendFile)

        QMetaObject.connectSlotsByName(SendFile)
    # setupUi

    def retranslateUi(self, SendFile):
        SendFile.setWindowTitle(QCoreApplication.translate("SendFile", u"Nightwood - Send File", None))
        self.logoLabel.setText("")
        self.logoTextLabel.setText(QCoreApplication.translate("SendFile", u"Nightwood", None))
        self.receivedButton.setText(QCoreApplication.translate("SendFile", u"Received Files", None))
        self.sentButton.setText(QCoreApplication.translate("SendFile", u"Sent Files", None))
        self.sendFileButton.setText(QCoreApplication.translate("SendFile", u"Send File", None))
        self.settingsButton.setText(QCoreApplication.translate("SendFile", u"Settings", None))
        self.pageTitle.setText(QCoreApplication.translate("SendFile", u"Send File", None))
        self.usernameLabel.setText(QCoreApplication.translate("SendFile", u"Send to username:", None))
        self.usernameInput.setPlaceholderText(QCoreApplication.translate("SendFile", u"Enter recipient's username", None))
        self.fileLabel.setText(QCoreApplication.translate("SendFile", u"Select File:", None))
        self.filePathInput.setPlaceholderText(QCoreApplication.translate("SendFile", u"Choose a file to send", None))
        self.browseButton.setText(QCoreApplication.translate("SendFile", u"Browse", None))
        self.fileDetailsLabel.setText("")
        self.sendButton.setText(QCoreApplication.translate("SendFile", u"Send File", None))
    # retranslateUi

