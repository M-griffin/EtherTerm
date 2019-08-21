/**
 * @brief Oblivion/2 XRM Unit Tests for SessionIO.
 * @return
 */

//#include "session_io.hpp"

#ifdef _WIN32
#include <UnitTest++.h>
#else
#include <unittest++/UnitTest++.h>
#endif

// C++ Standard
#include <iostream>
#include <string>
#include <stdexcept>


SUITE(XRMSessionIO)
{

    /**
     * @brief session_data is a shared_ptr that is passed along to other instances
     *        session_data also keeps track of
     * @return
     */


    /**
    * @brief Input Key Tests, Receives Characters 1 at a time, stores and processes.
    *        Only passing through or building esc sequences.
    * @return
    *
    TEST(InputKey_Empty)
    {
        // Normal Characters and sequences should pass through
        //session_ptr session;
        //SessionIO sess(session);
        SessionIO sess();

        std::string character_buffer = "";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK(result.compare(character_buffer) == 0);
    }*/

/*
    TEST(InputKey_Normal_Character)
    {
        // Normal Characters and sequences should pass through
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "A";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK(result.compare(character_buffer) == 0);
    }

    TEST(InputKey_Double_Character_Expect_Empty)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "AA";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK(result.compare("") == 0);
    }

    TEST(InputKey_UniCode_Character)
    {
        // Normal Unicode Characters and sequences should pass through
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "あ";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK(result.compare(character_buffer) == 0);
    }

    TEST(InputKey_Two_UniCode_Character_Expect_Empty)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "ああ";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK(result.compare("") == 0);
    }

    TEST(InputKey_Mix_Normal_And_UniCode_Character_Expect_Empty)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "あAあ";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK(result.compare("") == 0);
    }

    TEST(InputKey_Mix_Normal_And_UniCode_Characters2_Expect_Empty)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "Aあ";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK(result.compare("") == 0);
    }

    TEST(InputKey_Mix_Normal_And_UniCode_Character3_Expect_Empty)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "あA";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK(result.compare("") == 0);
    }

    TEST(InputKey_Escape_Expect_Empty_Then_Esc)
    {
        // First ESC waits for \0 to signal it's single ESC key
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        // Verify Second Sequence return the ESC to signal it's alone.
        character_buffer = "\0";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("\x1b") == 0));
    }

    TEST(InputKey_Escape_Sequence_Up_Arrow_Failure)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b[A";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));
    }

    TEST(InputKey_Escape_Sequence_Up_Arrow_Sequence)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "[";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "A";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("\x1b""up_arrow") == 0));
    }

    TEST(InputKey_Escape_Sequence_Dn_Arrow_Sequence)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "[";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "B";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("\x1b""dn_arrow") == 0));
    }

    TEST(InputKey_Escape_Sequence_Lt_Arrow_Sequence)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "[";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "D";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("\x1b""lt_arrow") == 0));
    }

    TEST(InputKey_Escape_Sequence_Rt_Arrow_Sequence)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "[";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "C";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("\x1b""rt_arrow") == 0));
    }

    TEST(InputKey_Escape_Sequence_Hardware_Up_Arrow_Sequence)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "O";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "A";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("\x1b""up_arrow") == 0));
    }



    TEST(InputKey_Escape_Sequence_Hardware_Dn_Arrow_Sequence)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "O";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "B";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("\x1b""dn_arrow") == 0));
    }

    TEST(InputKey_Escape_Sequence_Hardware_Lt_Arrow_Sequence)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "O";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "D";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("\x1b""lt_arrow") == 0));
    }

    TEST(InputKey_Escape_Sequence_Hardware_Rt_Arrow_Sequence)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "O";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "C";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("\x1b""rt_arrow") == 0));
    }

    TEST(InputKey_Escape_Sequence_Esc_Twice_Returns_Esc)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "\x1b";
        std::string result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("") == 0));

        character_buffer = "\x1b";
        result = sess.getKeyInput(character_buffer);
        CHECK((result.compare("\x1b") == 0));
    }*/


    /**
     * @brief Create Input Field Tests
     * @return
     */
     /*
    TEST(createInputField_Empty_Returns_Empty)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "";
        int length = 0;
        sess.createInputField(character_buffer, length);
        CHECK((character_buffer.compare("") == 0));
    }

    TEST(createInputField_0_Length_Retuns_Field_Name)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        std::string character_buffer = "Testing";
        int length = 0;
        sess.createInputField(character_buffer, length);
        CHECK((character_buffer.compare("Testing") == 0));
    }

    TEST(createInputField_1_Length_Retuns_Field_Name_1_Length_ANSI_Field)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer = "Testing ";
        int length = 1;
        sess.createInputField(character_buffer, length);
        CHECK((character_buffer.compare("Testing |00|19 \x1b[2D") == 0));
    }

    TEST(createInputField_10_Length_Retuns_Field_Name_10_Length_ANSI_Field)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer = "Testing ";
        int length = 10;
        sess.createInputField(character_buffer, length);
        CHECK((character_buffer.compare("Testing |00|19          \x1b[11D") == 0));
    }

    TEST(createInputField_10_Length_Retuns_Default_Becasue_FL17_Larger_Than_length10)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer = "Testing |FL17";
        int length = 10;
        sess.createInputField(character_buffer, length);
        CHECK((character_buffer.compare("Testing |00|19          \x1b[11D") == 0));
    }

    TEST(createInputField_10_Length_Retuns_Default_Becasue_FL10_Smaller_Than_length20)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer = "Testing |FL10";
        int length = 20;
        sess.createInputField(character_buffer, length);
        CHECK((character_buffer.compare("Testing |00|19          \x1b[11D") == 0));
    }

    TEST(createInputField_10_Length_Retuns_Default_Becasue_FL10_Larger_Than_length9_Failure)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer = "Testing |FL10";
        int length = 9;
        sess.createInputField(character_buffer, length);

        // Doesn't Equal 10, casue it's over the max!
        CHECK((character_buffer.compare("Testing |00|19          \x1b[12D") != 0));

        // Equals 9 the max
        CHECK((character_buffer.compare("Testing |00|19         \x1b[10D") == 0));
    }

    TEST(createInputField_20_Length_Retuns_Field_Name_Override_Input_Length)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer = "Testing |FL17";
        int length = 20;
        sess.createInputField(character_buffer, length);
        CHECK((character_buffer.compare("Testing |00|19                 \x1b[18D") == 0));
    }

    TEST(createInputField_20_Length_Retuns_Field_Name_Override_Input_Length_And_Color)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer = "Testing |FL17|FB0116";
        int length = 20;
        sess.createInputField(character_buffer, length);
        CHECK((character_buffer.compare("Testing |01|16                 \x1b[18D") == 0));
    }

    TEST(createInputField_10_Length_Retuns_Field_Name_Override_Color_Only)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer = "Testing: |FB0116";
        int length = 10;
        sess.createInputField(character_buffer, length);
        CHECK((character_buffer.compare("Testing: |01|16          \x1b[11D") == 0));
    }*/

    /**
     * @brief Get Input Field Tests
     * @return
     */
    /*
    TEST(getInputField_Test_Field_Return_No_LF)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer;
        std::string result = "";
        int length = 10;

        character_buffer = "T";
        std::string value = sess.getInputField(character_buffer, result, length);

        // Value is always returned to display back to the user.
        CHECK((value.compare("T") == 0));

        character_buffer = "E";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "S";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "T";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "\n";
        sess.getInputField(character_buffer, result, length);
        CHECK((result.compare("TEST") == 0));
    }

    TEST(getInputField_Test_Field_Return_No_LF_Received_Return_Empty)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer;
        std::string result = "";
        int length = 10;

        character_buffer = "T";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "E";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "S";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "T";
        sess.getInputField(character_buffer, result, length);
        CHECK((result.compare("") == 0));
    }

    TEST(getInputField_Test_Field_Return_Aborted_On_ESC)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer;
        std::string result = "";
        int length = 10;

        character_buffer = "\x1b";
        sess.getInputField(character_buffer, result, length);
        CHECK((result.compare("") == 0));

        character_buffer = "\0";
        result.clear();
        std::string value = sess.getInputField(character_buffer, result, length);

        CHECK_EQUAL(value, "aborted"); // String Aborted Return on ESC Press.
        CHECK_EQUAL(result, "");       // // Result Blank on Aborts only returns data after ENTER
    }

    TEST(getInputField_Test_Field_Return_Aborted_On_ESC_Mid_Field)
    {
        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer;
        std::string result = "";
        int length = 10;

        character_buffer = "T";
        std::string value = sess.getInputField(character_buffer, result, length);
        CHECK((value.compare("T") == 0));

        character_buffer = "E";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "S";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "T";
        sess.getInputField(character_buffer, result, length);


        character_buffer = "\x1b";
        sess.getInputField(character_buffer, result, length);
        CHECK((result.compare("") == 0));

        character_buffer = "\0";
        result.clear();
        value = sess.getInputField(character_buffer, result, length);

        CHECK_EQUAL(value, "aborted"); // String Aborted Return on ESC Press.
        CHECK_EQUAL(result, "");       // Result Blank on Aborts only returns data after ENTER
    }

    TEST(getInputField_Test_Field_Returns_Empty_On_ESC_Squences_Returns_Following_Character)
    {
        // We want to make sure ESC sequences, arrow keys etc..
        // will not be mistaken for ESC and abort the field.

        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer;
        std::string result = "";
        int length = 10;

        character_buffer = "T";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "E";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "S";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "T";
        sess.getInputField(character_buffer, result, length);


        character_buffer = "\x1b";
        std::string value = sess.getInputField(character_buffer, result, length);
        CHECK_EQUAL(value, "");       // String Aborted Return on ESC Press.
        CHECK_EQUAL(result, "");      // Result Blank on Aborts

        character_buffer = "[";
        result.clear();
        value = sess.getInputField(character_buffer, result, length);
        CHECK_EQUAL(value, "");       // String Aborted Return on ESC Press.
        CHECK_EQUAL(result, "");      // Result Blank on Aborts

        character_buffer = "A";
        result.clear();
        value = sess.getInputField(character_buffer, result, length);

        std::cout << value << std::endl;
        CHECK_EQUAL(value, "empty");  // String Aborted Return on ESC Press.
        CHECK_EQUAL(result, "");      // Result Blank on Aborts

        // Now return the character after the ESC Sequence.
        character_buffer = "A";
        result.clear();
        value = sess.getInputField(character_buffer, result, length);
        CHECK_EQUAL(value, "A");      // String Aborted Return on ESC Press.
        CHECK_EQUAL(result, "");      // Result Blank on Aborts only returns data after ENTER
    }

    TEST(getInputField_Test_Field_Returns_Result_String_On_Enter)
    {
        // We want to make sure ESC sequences, arrow keys etc..
        // will not be mistaken for ESC and abort the field.

        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer;
        std::string result = "";
        int length = 10;

        character_buffer = "T";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "E";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "S";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "T";
        sess.getInputField(character_buffer, result, length);

        character_buffer = "\n";
        std::string value = sess.getInputField(character_buffer, result, length);
        CHECK_EQUAL(value, "\n");  // String Aborted Return on ESC Press.
        CHECK_EQUAL(result, "TEST");        // Result Blank on Aborts
    }


    TEST(getInputField_Test_Field_Incomplete_ESC_ignored_Returns_Empty)
    {
        // We want to make sure ESC sequences, arrow keys etc..
        // will not be mistaken for ESC and abort the field.

        // Mock SessionData Class
        connection_ptr          connection;
        session_manager_ptr     room;
        boost::asio::io_service io_service;
        state_manager_ptr       state_manager;

        session_data_ptr session_data(new SessionData(connection, room, io_service, state_manager));
        SessionIO sess(session_data);

        std::string character_buffer;
        std::string result = "";
        int length = 10;


        character_buffer = "\x1b";
        std::string value = sess.getInputField(character_buffer, result, length);
        CHECK_EQUAL(value, "");    // String Aborted Return on ESC Press.
        CHECK_EQUAL(result, "");   // Result Blank on Aborts

        character_buffer = "\n";
        value = sess.getInputField(character_buffer, result, length);
        std::cout << "RESULT: " << result << " : " << value << std::endl;
        CHECK_EQUAL(value, "\n");  // String Aborted Return on ESC Press.
        CHECK_EQUAL(result, "");   // Result Blank on Aborts
    }


    // Start Testing CodeMaps

    TEST(pipe2codeMap_Test_MapCode_Group1)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // Test Group 1 ([|]{1}[0-9]{2})                // |00
        std::string sequence = "e|013e|02AA||03FE";

        std::vector<MapType> code_map = std::move(sess.parseToCodeMap(sequence, sess.STD_EXPRESSION));
        CHECK_EQUAL(code_map.size(), 3);

        // Verify Codes
        CHECK_EQUAL(code_map[0].m_code, "|01");
        CHECK_EQUAL(code_map[1].m_code, "|02");
        CHECK_EQUAL(code_map[2].m_code, "|03");

        // Verify Groups
        CHECK_EQUAL(code_map[0].m_match, 1);
        CHECK_EQUAL(code_map[1].m_match, 1);
        CHECK_EQUAL(code_map[2].m_match, 1);
    }

    TEST(pipe2codeMap_Test_MapCode_Group2)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // Test Group 2 ([|]{1}[X][Y][0-9]{4}           // |XY0101
        std::string sequence = "ers|XY0101e3%||XY101000weXa";

        std::vector<MapType> code_map = std::move(sess.parseToCodeMap(sequence, sess.STD_EXPRESSION));
        CHECK_EQUAL(code_map.size(), 2);

        // Verify Codes
        CHECK_EQUAL(code_map[0].m_code, "|XY0101");
        CHECK_EQUAL(code_map[1].m_code, "|XY1010");

        // Verify Groups
        CHECK_EQUAL(code_map[0].m_match, 2);
        CHECK_EQUAL(code_map[1].m_match, 2);
    }

    TEST(pipe2codeMap_Test_MapCode_Group3)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // Test Group 3 ([|]{1}[A-Z]{1,2}[0-9]{1,2})    // |A1 A22  AA2  AA33
        std::string sequence = "..W||A1%|B22XY1010weR-|AI3XY1010342|CC223eq0";

        std::vector<MapType> code_map = std::move(sess.parseToCodeMap(sequence, sess.STD_EXPRESSION));
        CHECK_EQUAL(code_map.size(), 4);

        // Verify Codes
        CHECK_EQUAL(code_map[0].m_code, "|A1");
        CHECK_EQUAL(code_map[1].m_code, "|B22");
        CHECK_EQUAL(code_map[2].m_code, "|AI3");
        CHECK_EQUAL(code_map[3].m_code, "|CC22");

        // Verify Groups
        CHECK_EQUAL(code_map[0].m_match, 3);
        CHECK_EQUAL(code_map[1].m_match, 3);
        CHECK_EQUAL(code_map[2].m_match, 3);
        CHECK_EQUAL(code_map[3].m_match, 3);
    }

    TEST(pipe2codeMap_Test_MapCode_Group4)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // Test Group 4 ([|]{1}[A-Z]{2})                // |AA
        std::string sequence = "..W||AE%|Be22XY1010weR-|AI|3XY1010342|CCe2%%C|ERE23eq0";

        std::vector<MapType> code_map = std::move(sess.parseToCodeMap(sequence, sess.STD_EXPRESSION));
        CHECK_EQUAL(code_map.size(), 4);

        // Verify Codes
        CHECK_EQUAL(code_map[0].m_code, "|AE");
        CHECK_EQUAL(code_map[1].m_code, "|AI");
        CHECK_EQUAL(code_map[2].m_code, "|CC");
        CHECK_EQUAL(code_map[3].m_code, "|ER");

        // Verify Groups
        CHECK_EQUAL(code_map[0].m_match, 4);
        CHECK_EQUAL(code_map[1].m_match, 4);
        CHECK_EQUAL(code_map[2].m_match, 4);
        CHECK_EQUAL(code_map[3].m_match, 4);
    }

    TEST(pipe2codeMap_Test_MapCode_Group5)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // Test Group 5 ([%]{2}[\w]+[.]{1}[\w]{3})      // %%filename.ans
        std::string sequence = "..W||1AE%|Be22%%obv.ascecXY1010%%%obv.txt%w%%obv.tx~";

        std::vector<MapType> code_map = std::move(sess.parseToCodeMap(sequence, sess.STD_EXPRESSION));
        CHECK_EQUAL(code_map.size(), 2);

        // Verify Codes
        CHECK_EQUAL(code_map[0].m_code, "%%obv.asc");
        CHECK_EQUAL(code_map[1].m_code, "%%obv.txt");

        // Verify Groups
        CHECK_EQUAL(code_map[0].m_match, 5);
        CHECK_EQUAL(code_map[1].m_match, 5);
    }

    TEST(pipe2codeMap_Test_MapCode_Group6)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // Test Group 6 ([%]{1}[A-Z]{2})                // %AA
        std::string sequence = "..W||1AE%1|Be22%%o$%TAbXY1010%%RF%obw%%obv.tx~";

        std::vector<MapType> code_map = std::move(sess.parseToCodeMap(sequence, sess.STD_EXPRESSION));
        CHECK_EQUAL(code_map.size(), 2);

        // Verify Codes
        CHECK_EQUAL(code_map[0].m_code, "%TA");
        CHECK_EQUAL(code_map[1].m_code, "%RF");

        // Verify Groups
        CHECK_EQUAL(code_map[0].m_match, 6);
        CHECK_EQUAL(code_map[1].m_match, 6);
    }

    TEST(pipe2codeMap_Test_MapCode_Group7)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // Test Group 7 ([%]{1}[0-9]{2})                // %11
        std::string sequence = "..W||1AE%1|Be22%%o1$%18bXY1010%%991%obw%%obv.tx~";

        std::vector<MapType> code_map = std::move(sess.parseToCodeMap(sequence, sess.STD_EXPRESSION));
        CHECK_EQUAL(code_map.size(), 2);

        // Verify Codes
        CHECK_EQUAL(code_map[0].m_code, "%18");
        CHECK_EQUAL(code_map[1].m_code, "%99");

        // Verify Groups
        CHECK_EQUAL(code_map[0].m_match, 7);
        CHECK_EQUAL(code_map[1].m_match, 7);
    }

    TEST(pipe2codeMap_Test_MapCode_AllGroups)
    {
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // Test all groups are picked up when combinded
        std::string sequence = "e|013e|02AA||03FEers|XY0101e3%||XY101000weXa..W" \
                               "||A1%|B22XY1010weR-|AI3XY1010342|CC223eq0..W||AE%|Be22XY1010weR-|A" \
                               "I|3XY1010342|CCe2%%C|ERE23eq0..W||1AE%|Be22%%obv.ascecXY1010%%%obv" \
                               ".txt%w%%obv.tx~..W||1AE%1|Be22%%o$%TAbXY1010%%RF%obw%%obv.tx~..W||" \
                               "1AE%1|Be22%%o1$%18bXY1010%%991%obw%%obv.tx~";

        std::vector<MapType> code_map = std::move(sess.parseToCodeMap(sequence, sess.STD_EXPRESSION));
        CHECK_EQUAL(code_map.size(), 19);

        // Verify Codes
        CHECK_EQUAL(code_map[0].m_code, "|01");
        CHECK_EQUAL(code_map[1].m_code, "|02");
        CHECK_EQUAL(code_map[2].m_code, "|03");
        CHECK_EQUAL(code_map[3].m_code, "|XY0101");
        CHECK_EQUAL(code_map[4].m_code, "|XY1010");
        CHECK_EQUAL(code_map[5].m_code, "|A1");
        CHECK_EQUAL(code_map[6].m_code, "|B22");
        CHECK_EQUAL(code_map[7].m_code, "|AI3");
        CHECK_EQUAL(code_map[8].m_code, "|CC22");
        CHECK_EQUAL(code_map[9].m_code, "|AE");
        CHECK_EQUAL(code_map[10].m_code, "|AI");
        CHECK_EQUAL(code_map[11].m_code, "|CC");
        CHECK_EQUAL(code_map[12].m_code, "|ER");
        CHECK_EQUAL(code_map[13].m_code, "%%obv.asc");
        CHECK_EQUAL(code_map[14].m_code, "%%obv.txt");
        CHECK_EQUAL(code_map[15].m_code, "%TA");
        CHECK_EQUAL(code_map[16].m_code, "%RF");
        CHECK_EQUAL(code_map[17].m_code, "%18");
        CHECK_EQUAL(code_map[18].m_code, "%99");

        // Verify Groups
        CHECK_EQUAL(code_map[0].m_match, 1);
        CHECK_EQUAL(code_map[1].m_match, 1);
        CHECK_EQUAL(code_map[2].m_match, 1);
        CHECK_EQUAL(code_map[3].m_match, 2);
        CHECK_EQUAL(code_map[4].m_match, 2);
        CHECK_EQUAL(code_map[5].m_match, 3);
        CHECK_EQUAL(code_map[6].m_match, 3);
        CHECK_EQUAL(code_map[7].m_match, 3);
        CHECK_EQUAL(code_map[8].m_match, 3);
        CHECK_EQUAL(code_map[9].m_match, 4);
        CHECK_EQUAL(code_map[10].m_match, 4);
        CHECK_EQUAL(code_map[11].m_match, 4);
        CHECK_EQUAL(code_map[12].m_match, 4);
        CHECK_EQUAL(code_map[13].m_match, 5);
        CHECK_EQUAL(code_map[14].m_match, 5);
        CHECK_EQUAL(code_map[15].m_match, 6);
        CHECK_EQUAL(code_map[16].m_match, 6);
        CHECK_EQUAL(code_map[17].m_match, 7);
        CHECK_EQUAL(code_map[18].m_match, 7);
    }

    // Test Regex Config Field Validations
    TEST(checkRegex_config_regexp_generic_validation_pass)
    {
        config_ptr config(new Config());
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // [^\\s][\\w\\s,.!@#$%^&*()]+
        std::string sequence = "123west,.!@#$%^&*()";
        std::string expression = config->regexp_generic_validation;

        bool result = sess.checkRegex(sequence, expression);
        CHECK_EQUAL(result, true);
    }

    TEST(checkRegex_config_regexp_generic_validation_no_leading_spaces)
    {
        config_ptr config(new Config());
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // [^\\s][\\w\\s,.!@#$%^&*()]+
        std::string sequence = " 123west";
        std::string expression = config->regexp_generic_validation;

        bool result = sess.checkRegex(sequence, expression);
        CHECK_EQUAL(result, false);
    }

    TEST(checkRegex_config_regexp_generic_validation_space_seperator)
    {
        config_ptr config(new Config());
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // [^\\s][\\w\\s,.!@#$%^&*()]+
        std::string sequence = "123west ,.!@#$%^&*()";
        std::string expression = config->regexp_generic_validation;

        bool result = sess.checkRegex(sequence, expression);
        CHECK_EQUAL(result, true);
    }

    TEST(checkRegex_config_regexp_generic_validation_no_leading_space2)
    {
        config_ptr config(new Config());
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // [^\\s][\\w\\s,.!@#$%^&*()]+
        std::string sequence = " 123west ,.!@#$%^&*()";
        std::string expression = config->regexp_generic_validation;

        bool result = sess.checkRegex(sequence, expression);
        CHECK_EQUAL(result, false);
    }
    */

    // \X Match any Unicode combining character sequence, for example "a\x 0301" (a letter a with an acute).
    // Boost is not setup properly for ICU and regex (WINDOWS),  fixme!
    /*
    TEST(checkRegex_config_regexp_generic_validation_test_unicode)
    {
        config_ptr config(new Config());
        session_data_ptr session_data;
        SessionIO sess(session_data);

        // [^\\s][\\w\\s,.!@#$%^&*()]+
        std::string sequence = "123west ,.!@#$%^&*() Ê î Î ô Ô û Û";
        std::string expression = config->regexp_generic_validation;

        bool result = sess.checkRegex(sequence, expression);
        CHECK_EQUAL(result, true);
    }*/

}
