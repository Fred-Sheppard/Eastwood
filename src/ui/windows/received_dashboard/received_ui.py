# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'received.ui'
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
class Ui_Received(object):
    def setupUi(self, Received):
        if not Received.objectName():
            Received.setObjectName(u"Received")
        Received.resize(800, 600)
        Received.setMinimumSize(QSize(800, 600))
        Received.setStyleSheet(u"background-color: #f5f6fa;")
        self.verticalLayout = QVBoxLayout(Received)
        self.verticalLayout.setSpacing(25)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(30, 30, 30, 30)
        self.headerLayout = QHBoxLayout()
        self.headerLayout.setSpacing(15)
        self.headerLayout.setObjectName(u"headerLayout")
        self.logoLabel = QLabel(Received)
        self.logoLabel.setObjectName(u"logoLabel")
        self.logoLabel.setMinimumSize(QSize(40, 40))
        self.logoLabel.setMaximumSize(QSize(40, 40))
        self.logoLabel.setStyleSheet(u"background-color: #6c5ce7;\n"
"border-radius: 8px;")

        self.headerLayout.addWidget(self.logoLabel)

        self.titleLabel = QLabel(Received)
        self.titleLabel.setObjectName(u"titleLabel")
        self.titleLabel.setStyleSheet(u"font-size: 28px; font-weight: bold; color: #2d3436; letter-spacing: 0.5px;")

        self.headerLayout.addWidget(self.titleLabel)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.headerLayout.addItem(self.horizontalSpacer)

        self.sentDashboardButton = QPushButton(Received)
        self.sentDashboardButton.setObjectName(u"sentDashboardButton")
        self.sentDashboardButton.setMinimumSize(QSize(120, 40))
        self.sentDashboardButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  color: #2d3436;\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 8px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #f1f2f6;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #dfe6e9;\n"
"}")

        self.headerLayout.addWidget(self.sentDashboardButton)

        self.sendButton = QPushButton(Received)
        self.sendButton.setObjectName(u"sendButton")
        self.sendButton.setMinimumSize(QSize(120, 40))
        self.sendButton.setStyleSheet(u"QPushButton {\n"
"  font-size: 14px;\n"
"  font-weight: bold;\n"
"  color: white;\n"
"  background-color: #6c5ce7;\n"
"  border-radius: 8px;\n"
"  border: none;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: #5049c9;\n"
"}\n"
"QPushButton:pressed {\n"
"  background-color: #4040b0;\n"
"}")

        self.headerLayout.addWidget(self.sendButton)


        self.verticalLayout.addLayout(self.headerLayout)

        self.sectionLabel = QLabel(Received)
        self.sectionLabel.setObjectName(u"sectionLabel")
        self.sectionLabel.setStyleSheet(u"font-size: 16px; color: #636e72; margin-top: 5px;")

        self.verticalLayout.addWidget(self.sectionLabel)

        self.fileList = QListWidget(Received)
        self.fileList.setObjectName(u"fileList")
        self.fileList.setStyleSheet(u"QListWidget {\n"
"  background-color: white;\n"
"  border: 1px solid #dfe6e9;\n"
"  border-radius: 10px;\n"
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
"  border-radius: 4px;\n"
"}\n"
"QScrollBar::handle:vertical {\n"
"  background: #dfe6e9;\n"
"  border-radius: 4px;\n"
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

        self.verticalLayout.addWidget(self.fileList)


        self.retranslateUi(Received)

        QMetaObject.connectSlotsByName(Received)
    # setupUi

    def retranslateUi(self, Received):
        Received.setWindowTitle(QCoreApplication.translate("Received", u"Nightwood - Received Files", None))
        self.logoLabel.setText("")
        self.titleLabel.setText(QCoreApplication.translate("Received", u"Nightwood", None))
        self.sentDashboardButton.setText(QCoreApplication.translate("Received", u"Sent Files", None))
        self.sendButton.setText(QCoreApplication.translate("Received", u"Send File", None))
        self.sectionLabel.setText(QCoreApplication.translate("Received", u"Received Files", None))
    # retranslateUi

