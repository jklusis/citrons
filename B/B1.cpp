#include <iostream>

using namespace std;

int integerB1(int integer);
int getIntegerLength(int integer);
int getIntegerInTenthPower(int integer, int power);

int main () 
{
    bool willContinue = true;
    while (willContinue) {
        int integer;

        cout << "Enter a positive integer" << endl;
        cin >> integer;

        if (integer < 0) {
            cout << "ok";
            
            return 13;
        }
        
        cout << "Entered integer: " << integer << endl;
        cout << "Processed integer (B1): " << integerB1(integer) << endl;

        cout << "Continue?" << endl; 
        cin >> willContinue;
    }
    
    cout << "Exiting";
}

int integerB1(int integer) 
{
    int length = getIntegerLength(integer);

    if (length == 0) {
        return 0;
    }

    // If 1, to flip it we simply need to multiply by 10
    if (length == 1) {
        return integer * 10;
    }

    int processedInteger = 0;

    for (int endPosition = 1; endPosition <= length; endPosition++) {
        // Get the current number and get rid of it in the integer;
        int currentNumber = integer % 10;
        integer /= 10;

        // Get the addition to power, if the last number, we need to multiply by ten, if prior to last, multiply by one
        int additionToPower;
        switch (endPosition) {
            case 1:
                additionToPower = 1;
                break;
            case 2:
                additionToPower = -1;
                break;
            default:
                additionToPower = 0;
        }

        // The added number will be number * 10^(positionFromTheEnd - 1)
        processedInteger += getIntegerInTenthPower(currentNumber, endPosition - 1 + additionToPower);
    }

    return processedInteger;
}

int getIntegerLength(int integer) 
{
    int length = 0;

    while (integer > 0) {
        length++;
        integer /= 10;
    }

    return length;
}

int getIntegerInTenthPower(int integer, int power)
{
    int multiplier = 1;

    while (power > 0) {
        multiplier *= 10;
        power -= 1;
    }

    return integer * multiplier;
}
