/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2011 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "TestInc.h"
#include "TestUtils.h"
#include "BaseTokenStreamFixture.h"
#include "RussianAnalyzer.h"
#include "RussianLetterTokenizer.h"
#include "InputStreamReader.h"
#include "FileReader.h"
#include "TermAttribute.h"
#include "StringReader.h"
#include "FileUtils.h"

using namespace Lucene;

typedef BaseTokenStreamFixture RussianAnalyzerTest;

TEST_F(RussianAnalyzerTest, testUnicode)
{
    RussianAnalyzerPtr ra = newLucene<RussianAnalyzer>(LuceneVersion::LUCENE_CURRENT);

    String testFile(FileUtils::joinPath(FileUtils::joinPath(getTestDir(), L"russian"), L"testUTF8.txt"));
    InputStreamReaderPtr inWords = newLucene<InputStreamReader>(newLucene<FileReader>(testFile));

    String sampleFile(FileUtils::joinPath(FileUtils::joinPath(getTestDir(), L"russian"), L"resUTF8.htm"));
    InputStreamReaderPtr sampleUnicode = newLucene<InputStreamReader>(newLucene<FileReader>(sampleFile));

    TokenStreamPtr in = ra->tokenStream(L"all", inWords);
    RussianLetterTokenizerPtr sample = newLucene<RussianLetterTokenizer>(sampleUnicode);

    TermAttributePtr text = in->getAttribute<TermAttribute>();
    TermAttributePtr sampleText = sample->getAttribute<TermAttribute>();

    while (true)
    {
        if (!in->incrementToken())
            break;
        sample->incrementToken();
        EXPECT_EQ(text->term(), sampleText->term());
    }

    inWords->close();
    sampleUnicode->close();
}

TEST_F(RussianAnalyzerTest, testDigitsInRussianCharset)
{
    ReaderPtr reader = newLucene<StringReader>(L"text 1000");
    RussianAnalyzerPtr ra = newLucene<RussianAnalyzer>(LuceneVersion::LUCENE_CURRENT);
    TokenStreamPtr stream = ra->tokenStream(L"", reader);

    TermAttributePtr termText = stream->getAttribute<TermAttribute>();
    EXPECT_TRUE(stream->incrementToken());
    EXPECT_EQ(L"text", termText->term());
    EXPECT_TRUE(stream->incrementToken());
    EXPECT_EQ(L"1000", termText->term());
    EXPECT_TRUE(!stream->incrementToken());
}

TEST_F(RussianAnalyzerTest, testReusableTokenStream1)
{
    AnalyzerPtr a = newLucene<RussianAnalyzer>(LuceneVersion::LUCENE_CURRENT);

    const uint8_t input[] =
    {
        0xd0, 0x92, 0xd0, 0xbc, 0xd0, 0xb5, 0xd1, 0x81, 0xd1, 0x82, 0xd0, 0xb5, 0x20, 0xd1, 0x81, 0x20,
        0xd1, 0x82, 0xd0, 0xb5, 0xd0, 0xbc, 0x20, 0xd0, 0xbe, 0x20, 0xd1, 0x81, 0xd0, 0xb8, 0xd0, 0xbb,
        0xd0, 0xb5, 0x20, 0xd1, 0x8d, 0xd0, 0xbb, 0xd0, 0xb5, 0xd0, 0xba, 0xd1, 0x82, 0xd1, 0x80, 0xd0,
        0xbe, 0xd0, 0xbc, 0xd0, 0xb0, 0xd0, 0xb3, 0xd0, 0xbd, 0xd0, 0xb8, 0xd1, 0x82, 0xd0, 0xbd, 0xd0,
        0xbe, 0xd0, 0xb9, 0x20, 0xd1, 0x8d, 0xd0, 0xbd, 0xd0, 0xb5, 0xd1, 0x80, 0xd0, 0xb3, 0xd0, 0xb8,
        0xd0, 0xb8, 0x20, 0xd0, 0xb8, 0xd0, 0xbc, 0xd0, 0xb5, 0xd0, 0xbb, 0xd0, 0xb8, 0x20, 0xd0, 0xbf,
        0xd1, 0x80, 0xd0, 0xb5, 0xd0, 0xb4, 0xd1, 0x81, 0xd1, 0x82, 0xd0, 0xb0, 0xd0, 0xb2, 0xd0, 0xbb,
        0xd0, 0xb5, 0xd0, 0xbd, 0xd0, 0xb8, 0xd0, 0xb5, 0x20, 0xd0, 0xb5, 0xd1, 0x89, 0xd0, 0xb5
    };

    const uint8_t token1[] = {0xd0, 0xb2, 0xd0, 0xbc, 0xd0, 0xb5, 0xd1, 0x81, 0xd1, 0x82};
    const uint8_t token2[] = {0xd1, 0x81, 0xd0, 0xb8, 0xd0, 0xbb};
    const uint8_t token3[] = {0xd1, 0x8d, 0xd0, 0xbb, 0xd0, 0xb5, 0xd0, 0xba, 0xd1, 0x82, 0xd1, 0x80, 0xd0,
                              0xbe, 0xd0, 0xbc, 0xd0, 0xb0, 0xd0, 0xb3, 0xd0, 0xbd, 0xd0, 0xb8, 0xd1, 0x82,
                              0xd0, 0xbd};
    const uint8_t token4[] = {0xd1, 0x8d, 0xd0, 0xbd, 0xd0, 0xb5, 0xd1, 0x80, 0xd0, 0xb3};
    const uint8_t token5[] = {0xd0, 0xb8, 0xd0, 0xbc, 0xd0, 0xb5, 0xd0, 0xbb};
    const uint8_t token6[] = {0xd0, 0xbf, 0xd1, 0x80, 0xd0, 0xb5, 0xd0, 0xb4, 0xd1, 0x81, 0xd1, 0x82, 0xd0,
                              0xb0, 0xd0, 0xb2, 0xd0, 0xbb, 0xd0, 0xb5, 0xd0, 0xbd};

    checkAnalyzesToReuse(a, UTF8_TO_STRING(input), newCollection<String>(
                            UTF8_TO_STRING(token1),
                            UTF8_TO_STRING(token2),
                            UTF8_TO_STRING(token3),
                            UTF8_TO_STRING(token4),
                            UTF8_TO_STRING(token5),
                            UTF8_TO_STRING(token6)
                         ));
}

TEST_F(RussianAnalyzerTest, testReusableTokenStream2)
{
    AnalyzerPtr a = newLucene<RussianAnalyzer>(LuceneVersion::LUCENE_CURRENT);

    const uint8_t input[] =
    {
        0xd0, 0x9d, 0xd0, 0xbe, 0x20, 0xd0, 0xb7, 0xd0, 0xbd, 0xd0, 0xb0, 0xd0, 0xbd, 0xd0, 0xb8, 0xd0,
        0xb5, 0x20, 0xd1, 0x8d, 0xd1, 0x82, 0xd0, 0xbe, 0x20, 0xd1, 0x85, 0xd1, 0x80, 0xd0, 0xb0, 0xd0,
        0xbd, 0xd0, 0xb8, 0xd0, 0xbb, 0xd0, 0xbe, 0xd1, 0x81, 0xd1, 0x8c, 0x20, 0xd0, 0xb2, 0x20, 0xd1,
        0x82, 0xd0, 0xb0, 0xd0, 0xb9, 0xd0, 0xbd, 0xd0, 0xb5
    };

    const uint8_t token1[] = {0xd0, 0xb7, 0xd0, 0xbd, 0xd0, 0xb0, 0xd0, 0xbd};
    const uint8_t token2[] = {0xd1, 0x85, 0xd1, 0x80, 0xd0, 0xb0, 0xd0, 0xbd};
    const uint8_t token3[] = {0xd1, 0x82, 0xd0, 0xb0, 0xd0, 0xb9, 0xd0, 0xbd};

    checkAnalyzesToReuse(a, UTF8_TO_STRING(input), newCollection<String>(
                            UTF8_TO_STRING(token1),
                            UTF8_TO_STRING(token2),
                            UTF8_TO_STRING(token3)
                         ));
}
