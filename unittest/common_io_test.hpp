/**
 * @brief Oblivion/2 XRM Unit Tests for CommonIO.
 * @return
 */

#include "common_io.hpp"

#ifdef _WIN32
#include <UnitTest++.h>
#else
#include <unittest++/UnitTest++.h>
#endif

// C++ Standard
#include <iostream>
#include <string>
#include <stdexcept>

/**
 * @brief Test Suit for CommonIO Class.
 * @return
 */
SUITE(ETCommonIO)
{
    TEST(ProgramPath)
    {
        CommonIO common;
        std::string myPath = common.getProgramPath("xrm-server");
        CHECK(!myPath.empty());
    }

    TEST(NumberOfCharacters)
    {
        CommonIO common;
        std::string temp = "testing";
        int length = common.numberOfChars(temp);
        CHECK_EQUAL(length,7);
    }

    TEST(NumberOfCharactersEmpty)
    {
        CommonIO common;
        std::string temp = "";
        int length = common.numberOfChars(temp);
        CHECK_EQUAL(length,0);
    }

    TEST(NumberOfCharactersUTF8)
    {
        CommonIO common;
        std::string temp = "あにま! Linux";
        int length = common.numberOfChars(temp);
        CHECK_EQUAL(length,10); // TODO Error..  Expected 13 but was 10
    }

    /**
     * @brief Left of string, remove spaces.
     * @return
     */
    TEST(LeftTrim)
    {
        CommonIO common;
        std::string temp = "   Linux   ";
        std::string left_temp = common.leftTrim(temp);
        CHECK_EQUAL(left_temp,"Linux   ");
    }

    TEST(LeftTrimEmpty)
    {
        CommonIO common;
        std::string temp = "";
        std::string left_temp = common.leftTrim(temp);
        CHECK_EQUAL(left_temp,"");
    }

    /**
     * @brief Right of String Remove spaces
     * @return
     */
    TEST(RightTrim)
    {
        CommonIO common;
        std::string temp = "   Linux   ";
        std::string right_temp = common.rightTrim(temp);
        CHECK_EQUAL(right_temp,"   Linux");
    }

    TEST(RightTrimEmpty)
    {
        CommonIO common;
        std::string temp = "";
        std::string right_temp = common.rightTrim(temp);
        CHECK_EQUAL(right_temp,"");
    }

    /**
     * @brief Remove all spaces left or right of text.
     * @return
     */
    TEST(Trim)
    {
        CommonIO common;
        std::string temp = "   Linux   ";
        std::string trim_temp = common.trim(temp);
        CHECK_EQUAL(trim_temp,"Linux");
    }

    TEST(TrimEmpty)
    {
        CommonIO common;
        std::string temp = "";
        std::string trim_temp = common.trim(temp);
        CHECK_EQUAL(trim_temp,"");
    }

    /**
     * @brief Erase Data in a String w/ start, end range.
     * @return
     */
    TEST(EraseString)
    {
        CommonIO common;
        std::string temp = "   Linux----";
        std::string trim_temp = common.eraseString(temp, 6);
        CHECK_EQUAL(trim_temp,"   Lin");
    }

    TEST(EraseString0)
    {
        CommonIO common;
        std::string temp = "   Linux----";
        std::string trim_temp = common.eraseString(temp, 0);
        CHECK_EQUAL(trim_temp,"");
    }

    TEST(EraseString_Empty)
    {
        CommonIO common;
        std::string temp = "";
        std::string trim_temp = common.eraseString(temp, 6);
        CHECK_EQUAL(trim_temp,"");
    }

    TEST(EraseString_Range)
    {
        CommonIO common;
        std::string temp = "testing";
        std::string trim_temp = common.eraseString(temp, 0, 4);
        std::cout << "[" << trim_temp << "]" << std::endl;
        CHECK_EQUAL(trim_temp,"ing");
    }

    TEST(EraseString_Range_Past)
    {
        CommonIO common;
        std::string temp = "testing";
        std::string trim_temp = common.eraseString(temp, 2, 20);
        std::cout << "[" << trim_temp << "]" << std::endl;
        CHECK_EQUAL(trim_temp,"te");
    }

    TEST(EraseString_Range_UTF8)
    {
        CommonIO common;
        std::string temp = "あにま! Lin";
        std::string trim_temp = common.eraseString(temp, 0, 2);
        std::cout << "[" << trim_temp << "]" << std::endl;
        CHECK_EQUAL(trim_temp,"ま! Lin"); // TODO Expected π½½π╛╛! Lin but was πü╛! Lin
    }

    TEST(EraseString_Range_UTF8_2)
    {
        CommonIO common;
        std::string temp = "あにま! Lin";
        std::string trim_temp = common.eraseString(temp, 1, 4);
        std::cout << "[" << trim_temp << "]" << std::endl;
        CHECK_EQUAL(trim_temp,"あLin"); //  TODO Expected π╛╛! Lin but was πüéLin
    }

    /**
     * @brief Pad Sapces on the right of the string, also truncates for absolute field lengths.
     * @return
     */
    TEST(RightPadding)
    {
        CommonIO common;
        std::string temp = "---";
        std::string padd_temp = common.rightPadding(temp, 6);
        CHECK_EQUAL(padd_temp,"---   ");
    }

    TEST(RightPadding_Truncate)
    {
        CommonIO common;
        std::string temp = "------";
        std::string padd_temp = common.rightPadding(temp, 3);
        CHECK_EQUAL(padd_temp,"---");
    }

    TEST(RightPadding_Empty)
    {
        CommonIO common;
        std::string temp = "";
        std::string padd_temp = common.rightPadding(temp, 3);
        CHECK_EQUAL(padd_temp,"");
    }

    TEST(RightPadding0)
    {
        CommonIO common;
        std::string temp = "---";
        std::string padd_temp = common.rightPadding(temp, 0);
        CHECK_EQUAL(padd_temp,"---");
    }

    /**
     * @brief Pad Sapces on the Left of the string, also truncates for absolute field lengths.
     * @return
     */
    TEST(LeftPadding)
    {
        CommonIO common;
        std::string temp = "---";
        std::string padd_temp = common.leftPadding(temp, 6);
        CHECK_EQUAL(padd_temp,"   ---");
    }

    TEST(LeftPadding_Truncate)
    {
        CommonIO common;
        std::string temp = "------ ";
        std::string padd_temp = common.leftPadding(temp, 3);
        CHECK_EQUAL(padd_temp,"-- ");
    }

    TEST(LeftPadding_Empty)
    {
        CommonIO common;
        std::string temp = "";
        std::string padd_temp = common.leftPadding(temp, 3);
        CHECK_EQUAL(padd_temp,"");
    }

    TEST(LeftPadding0)
    {
        CommonIO common;
        std::string temp = "---";
        std::string padd_temp = common.leftPadding(temp, 0);
        CHECK_EQUAL(padd_temp,"---");
    }

    /**
     * @brief Masks Field data returned, for passwords etc..
     * @return
     */
    TEST(MaskString_Empty)
    {
        CommonIO common;
        std::string temp = "";
        std::string mask_temp = common.maskString(temp);
        CHECK_EQUAL(mask_temp,"");
    }

    TEST(MaskString)
    {
        CommonIO common;
        std::string temp = "testing";
        std::string mask_temp = common.maskString(temp);
        CHECK_EQUAL(mask_temp,"*******");
    }

    /**
     * @brief Tests if string is a digit or number value.
     * @return
     *
    TEST(IsDigit_Bad_Preceeding)
    {
        CommonIO common;
        std::string temp = "a1";
        bool result = common.isDigit(temp);
        CHECK_EQUAL(result, false);
    }

    TEST(IsDigit_Bad_After)
    {
        CommonIO common;
        std::string temp = "1a";
        bool result = common.isDigit(temp);
        CHECK_EQUAL(result, false);
    }

    TEST(IsDigit)
    {
        CommonIO common;
        std::string temp = "1";
        bool result = common.isDigit(temp);
        CHECK_EQUAL(result, true);
    }

    TEST(IsDigit_Multiple)
    {
        CommonIO common;
        std::string temp = "125";
        bool result = common.isDigit(temp);
        CHECK_EQUAL(result, true);
    }

    TEST(IsDigit_Unicode_Bad_After)
    {
        CommonIO common;
        std::string temp = "६a";
        bool result = common.isDigit(temp);
        CHECK_EQUAL(result, false);
    }

    TEST(isDigit_Unicode_Multiple)
    {
        CommonIO common;
        std::string temp = "६೬";
        bool result = common.isDigit(temp);
        CHECK_EQUAL(result, true);
    }

    TEST(isDigit_Unicode_Bad_Preceeding)
    {
        CommonIO common;
        std::string temp = "a६ ೬";
        bool result = common.isDigit(temp);
        CHECK_EQUAL(result, false);
    }*/

    /**
    * @brief Parses Input char at a time.
    * @return
    */

    TEST(parseInput_Normal_Character_Should_Return_Same)
    {
        CommonIO common;
        std::string temp = "T";
        std::string result = common.parseInput(temp);
        CHECK_EQUAL(result, "T");
    }

    TEST(parseInput_ESC_Character_Should_Return_Empty)
    {
        CommonIO common;
        std::string temp = "\x1b";
        std::string result = common.parseInput(temp);
        CHECK_EQUAL(result, "");
    }

    TEST(parseInput_ESC_Character_Should_Return_Empty_Null_Returns_ESC)
    {
        CommonIO common;
        std::string temp = "\x1b";
        std::string result = common.parseInput(temp);
        CHECK_EQUAL(result, "");

        temp = "\0";
        result = common.parseInput(temp);
        CHECK_EQUAL(result, "\x1b");
    }

    TEST(parseInput_Multiple_Characters_Returns_Empty)
    {
        CommonIO common;
        std::string temp = "AB";
        std::string result = common.parseInput(temp);
        CHECK_EQUAL(result, "");
    }

    TEST(parseInput_Unicode_Character_Returns_Unicode_Character)
    {
        CommonIO common;
        std::string temp = "あ";
        std::string result = common.parseInput(temp);
        CHECK_EQUAL(result, "あ"); // TODO Expected  but was πüé
    }

    TEST(parseInput_Multiple_Unicode_Character_Returns_Empty)
    {
        CommonIO common;
        std::string temp = "ああ";
        std::string result = common.parseInput(temp);
        CHECK_EQUAL(result, "");
    }

    TEST(parseInput_ESC_With_Termination_Null_Returns_ESC)
    {
        CommonIO common;
        std::string temp = "\x1b";
        std::string result = common.parseInput(temp);
        CHECK_EQUAL(result, "");

        temp = "\0";
        result = common.parseInput(temp);
        CHECK_EQUAL(result, "\x1b");
    }


}
