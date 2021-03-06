/*****************************************************************************
 *
 * Created: 2016 by Eetu Kahelin / eekkelund
 *
 * Copyright 2016 Eetu Kahelin. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
*****************************************************************************/
#include "realhighlighter.h"

#include <QtGui>


RealHighlighter::RealHighlighter(QTextDocument *parent): QSyntaxHighlighter(parent)
{


}
void RealHighlighter::loadDict(QString path, QStringList &patterns){
    QFile dict(path);
    if (dict.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&dict);
        while (true)
        {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                patterns.append("\\b"+line+"\\b");
        }
        dict.close();
    }
}

void RealHighlighter::ruleUpdate()
{
    HighlightingRule rule;
    highlightingRules.clear();
    QStringList keywordPatterns;
    QStringList propertiesPatterns;

    if (m_dictionary=="qml") {
        jsFormat.setForeground(QColor(m_secondaryHighlightColor));
        jsFormat.setFontItalic(true);
        QStringList jsPatterns;
        loadDict(":/dictionaries/javascript.txt",jsPatterns);

        foreach (const QString &pattern, jsPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = jsFormat;
            highlightingRules.append(rule);
        }

        qmlFormat.setForeground(QColor(m_highlightColor));
        qmlFormat.setFontWeight(QFont::Bold);
        QStringList qmlPatterns;
        loadDict(":/dictionaries/qml.txt",qmlPatterns);

        foreach (const QString &pattern, qmlPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = qmlFormat;
            highlightingRules.append(rule);
        }

        keywordFormat.setForeground(QColor(m_highlightDimmerColor));
        keywordFormat.setFontWeight(QFont::Bold);

        loadDict(":/dictionaries/keywords.txt",keywordPatterns);

        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }
        propertiesFormat.setForeground(QColor(m_primaryColor));
        propertiesFormat.setFontWeight(QFont::Bold);

        loadDict(":/dictionaries/properties.txt",propertiesPatterns);

        foreach (const QString &pattern, propertiesPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = propertiesFormat;
            highlightingRules.append(rule);
        }

    }else if (m_dictionary=="py") {
        pythonFormat.setForeground(QColor(m_secondaryHighlightColor));
        pythonFormat.setFontItalic(true);
        QStringList pythonPatterns;
        loadDict(":/dictionaries/python.txt",pythonPatterns);

        foreach (const QString &pattern, pythonPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = pythonFormat;
            highlightingRules.append(rule);
        }

        keywordFormat.setForeground(QColor(m_highlightDimmerColor));
        keywordFormat.setFontWeight(QFont::Bold);
        loadDict(":/dictionaries/keywords.txt",keywordPatterns);

        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }
   }else if (m_dictionary=="js") {
        jsFormat.setForeground(QColor(m_secondaryHighlightColor));
        jsFormat.setFontItalic(true);
        QStringList jsPatterns;
        loadDict(":/dictionaries/javascript.txt",jsPatterns);

        foreach (const QString &pattern, jsPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = jsFormat;
            highlightingRules.append(rule);
        }
        keywordFormat.setForeground(QColor(m_highlightDimmerColor));
        keywordFormat.setFontWeight(QFont::Bold);
        loadDict(":/dictionaries/keywords.txt",keywordPatterns);

        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }
    }else{
        keywordFormat.setForeground(QColor(m_highlightDimmerColor));
        keywordFormat.setFontWeight(QFont::Bold);
        loadDict(":/dictionaries/keywords.txt",keywordPatterns);

        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }
    }
}

void RealHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    QTextCharFormat tmpFormat;
    enum {
        StartState = 0,
        NumberState = 1,
        IdentifierState = 2,
        StringState = 3,
        CommentState = 4
    };
    /*ORIGINAL FUNCTION TAKEN FROM HERE: https://github.com/olegyadrov/qmlcreator
    *ORIGINAL LICENSE APACHE2 AND CREATOR Oleg Yadrov
    *I HAVE MODIFIED ORIGINAL FUNCTION :)
    */
    QList<int> bracketPositions;

    int blockState = previousBlockState();
    int bracketLevel = blockState >> 4;
    int state = blockState & 15;
    if (blockState < 0) {
        bracketLevel = 0;
        state = StartState;
    }

    int start = 0;
    int i = 0;
    while (i <= text.length()) {
        QChar ch = (i < text.length()) ? text.at(i) : QChar();
        QChar next = (i < text.length() - 1) ? text.at(i + 1) : QChar();

        switch (state) {

        case StartState:
            start = i;
            if (ch.isSpace()) {
                ++i;
            } else if (ch.isDigit()) {
                ++i;
                state = NumberState;
            } else if (ch.isLetter() || ch == '_') {
                ++i;
                state = IdentifierState;
            } else if (ch == '\'' || ch == '\"') {
                ++i;
                state = StringState;
            } else if (ch == '/' && next == '*') {
                ++i;
                ++i;
                state = CommentState;
            } else if (ch == '/' && next == '/') {
                i = text.length();
                tmpFormat.setFontItalic(true);
                tmpFormat.setForeground(QColor(m_highlightBackgroundColor));
                setFormat(start, text.length(), tmpFormat);
            } else {
                if (!QString("(){}[]").contains(ch))
                    //setFormat(start, 1, Qt::green);
                if (ch =='{' || ch == '}') {
                    bracketPositions += i;
                    if (ch == '{')
                        bracketLevel++;
                    else
                        bracketLevel--;
                }
                ++i;
                state = StartState;
            }
            break;

        case NumberState:
            if (ch.isSpace() || !ch.isDigit()) {
                setFormat(start, i - start, QColor(m_primaryColor));
                state = StartState;
            } else {
                ++i;
            }
            break;

        /*case IdentifierState:
            if (ch.isSpace() || !(ch.isDigit() || ch.isLetter() || ch == '_')) {
                QString token = text.mid(start, i - start).trimmed();
                if (keywordPatterns.contains(token))
                    setFormat(start, i - start, Qt::darkBlue);
                //else if (m_qmlIdsCache.contains(token) || m_qmlIds.contains(token))
                    setFormat(start, i - start, Qt::darkBlue);
                //else if (m_propertiesCache.contains(token))
                    setFormat(start, i - start, Qt::darkBlue);
                //else if (m_jsIdsCache.contains(token) || m_jsIds.contains(token))
                    setFormat(start, i - start, Qt::darkBlue);
                state = StartState;
            } else {
                ++i;
            }
            break;*/

        case StringState:
            if (ch == text.at(start)) {
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                if (prev != '\\') {
                    ++i;
                    tmpFormat.setFontItalic(true);
                    tmpFormat.setForeground(QColor(m_secondaryColor));
                    setFormat(start, i - start, tmpFormat);
                    state = StartState;
                } else {
                    ++i;
                }
            } else {
                ++i;
            }
            break;
        case CommentState:
            if (ch == '*' && next == '/') {
                ++i;
                ++i;
                tmpFormat.setFontItalic(true);
                tmpFormat.setForeground(QColor(m_highlightBackgroundColor));
                setFormat(start, i - start, tmpFormat);
                state = StartState;
            } else {
                ++i;
            }
            break;
        default:
            state = StartState;
            break;
        }
    }

    if (state == CommentState){
        tmpFormat.setFontItalic(true);
        tmpFormat.setForeground(QColor(m_highlightBackgroundColor));
        setFormat(start, text.length(), tmpFormat);
    }
    else
        state = StartState;

    /*if (!m_markString.isEmpty()) {
        int pos = 0;
        int len = m_markString.length();
        QTextCharFormat markerFormat;
        markerFormat.setBackground(Qt::darkBlue);
        markerFormat.setForeground(Qt::darkBlue);
        for (;;) {
            pos = text.indexOf(m_markString, pos, m_markCaseSensitivity);
            if (pos < 0)
                break;
            setFormat(pos, len, markerFormat);
            ++pos;
        }
    }*/

    blockState = (state & 15) | (bracketLevel << 4);
    setCurrentBlockState(blockState);
}
void RealHighlighter::setStyle(QString primaryColor, QString secondaryColor, QString highlightColor, QString secondaryHighlightColor, QString highlightBackgroundColor, QString highlightDimmerColor, qreal baseFontPointSize)
{
    m_primaryColor = QString(primaryColor);
    m_secondaryColor = QString(secondaryColor);
    m_highlightColor = QString(highlightColor);
    m_secondaryHighlightColor = QString(secondaryHighlightColor);
    m_highlightBackgroundColor = QString(highlightBackgroundColor);
    m_highlightDimmerColor = QString(highlightDimmerColor);
    m_baseFontPointSize = baseFontPointSize;
    this->ruleUpdate();
    this->rehighlight();
}

void RealHighlighter::setDictionary(QString dictionary)
{
    m_dictionary = dictionary;
    this->ruleUpdate();
    this->rehighlight();
}

