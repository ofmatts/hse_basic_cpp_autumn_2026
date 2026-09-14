#include <iostream>
#include <string>
#include <cassert>
#include <cctype>


std::string map_alphabet_full = "ABCDEFGHIJKLMNOPQRSTVWXYZ"; 
//std::string map_alphabet = "BFPV,CGJKQSXZ,DT,L,MN,R";
std::string map_alphabet = "BFPVCGJKQSXZDTLMNR"; // индекс -> цифра

std::string convertTextToSound(std::string text)
{
    if (text.empty())
        return "0000"; // тривиальный случай? или выкидывать исключение?  
    
    if (text.length() < 4) text.resize(4, '0');

    std::string result;
    result += toupper(text[0]);
    char prevDigit = '0';
    for (int i = 1; i < text.length(); i++) 
    {
        char upperCh = toupper(text[i]);
        if (upperCh == 'H' || upperCh == 'W') continue; // без скипания HW эшкрафт не заработал

        int npos = map_alphabet.find(upperCh);

        if (npos == -1){
            prevDigit = '0';
            continue;
        }

        char digit;
        if (npos >= 0 && npos <= 3) digit = '1';
        else if (npos >= 4 && npos <= 11) digit = '2';
        else if (npos >= 12 && npos <= 13) digit = '3';
        else if (npos == 14) digit = '4';
        else if (npos >= 15 && npos <= 16) digit = '5';
        else digit = '6';

        if (digit == prevDigit) continue;

        result += digit;
        prevDigit = digit;

        if (result.length() >= 4) break;
    }

    
    while (result.length() < 4) result += '0';

    result = result.substr(0, 4);
    return result;
}

bool isEqual(std::string text1, std::string text2)
{
    return convertTextToSound(text1) == convertTextToSound(text2);
}

int main()
{
    assert( convertTextToSound("Andrey") == std::string{"A536"} );
    assert( convertTextToSound("Korneev") == std::string{"K651"} );

    assert( convertTextToSound("ASHCRAFT") == std::string{"A261"} );
    assert( convertTextToSound("Ashcraft") == std::string{"A261"} );
    assert( convertTextToSound("ashcraft") == std::string{"A261"} );

    assert( convertTextToSound("CAT") == std::string{"C300"} );
    assert( convertTextToSound("cat") == std::string{"C300"} );
    assert( convertTextToSound("") == std::string{"0000"} );

    assert( !isEqual("Andrey", "Korneev") );
    assert( isEqual("ASHCRAFT", "Ashcraft") );
    assert( isEqual("Ashcraft", "ashcraft") );
    assert( isEqual("ASHCRAFT", "ashcraft") );
    assert( !isEqual("USA", "RUSSIA") );
    assert( isEqual("CAT", "cat") );
    assert( !isEqual("CAT", "dog") );
    assert( isEqual("", "") );
    assert( !isEqual("", "cat") );

    std::cout << "test passed." << std::endl;
    return 0;
}