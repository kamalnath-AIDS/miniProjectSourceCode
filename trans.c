// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>

// structure to store client details
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // last name
    char firstName[10];   // first name
    double balance;       // balance
};

// function declarations
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // file pointer
    unsigned int choice; // user choice

    // open file in read/write binary mode
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(-1); // terminate if file not found
    }

    // menu loop until user chooses exit
    while ((choice = enterChoice()) != 5)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr); // create text file
            break;

        case 2:
            updateRecord(cfPtr); // update existing record
            break;

        case 3:
            newRecord(cfPtr); // add new record
            break;

        case 4:
            deleteRecord(cfPtr); // delete record
            break;

        default:
            puts("Incorrect choice"); // invalid option
            break;
        }
    }

    fclose(cfPtr); // close file
}

// create text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // pointer for text file
    int result;
    struct clientData client = {0, "", "", 0.0};

    // open text file
    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    }
    else
    {
        rewind(readPtr); // go to beginning

        // write headings
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
                "Acct", "Last Name", "First Name", "Balance");

        // read and copy records
        while (!feof(readPtr))
        {
            result = fread(&client, sizeof(struct clientData), 1, readPtr);

            // write only valid records
            if (result != 0 && client.acctNum != 0)
            {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n",
                        client.acctNum,
                        client.lastName,
                        client.firstName,
                        client.balance);
            }
        }

        fclose(writePtr); // close text file
    }
}

// update an existing record
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};

    // get account number
    printf("Enter account to update ( 1 - 100 ): ");
    scanf("%d", &account);

    // move pointer to correct record
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);

    // read record
    fread(&client, sizeof(struct clientData), 1, fPtr);

    // check if record exists
    if (client.acctNum == 0)
    {
        printf("Account #%d has no information.\n", account);
    }
    else
    {
        // display old data
        printf("%-6d%-16s%-11s%10.2f\n\n",
               client.acctNum, client.lastName, client.firstName, client.balance);

        // get transaction amount
        printf("Enter charge ( + ) or payment ( - ): ");
        scanf("%lf", &transaction);

        // update balance
        client.balance += transaction;

        // display updated data
        printf("%-6d%-16s%-11s%10.2f\n",
               client.acctNum, client.lastName, client.firstName, client.balance);

        // move pointer back to overwrite same record
        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);

        // write updated record
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

// delete a record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0};
    unsigned int accountNum;

    // get account number
    printf("Enter account number to delete ( 1 - 100 ): ");
    scanf("%d", &accountNum);

    // move to record
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    // read record
    fread(&client, sizeof(struct clientData), 1, fPtr);

    // check existence
    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    }
    else
    {
        // overwrite with blank record
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
}

// add new record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    // get new account number
    printf("Enter new account number ( 1 - 100 ): ");
    scanf("%d", &accountNum);

    // move to position
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    // check if already exists
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account #%d already contains information.\n", client.acctNum);
    }
    else
    {
        // input new data
        printf("Enter lastname, firstname, balance\n? ");
        scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);

        client.acctNum = accountNum;

        // write new record
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

// menu function
unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    // display menu
    printf("\nEnter your choice\n"
           "1 - store accounts.txt\n"
           "2 - update account\n"
           "3 - add account\n"
           "4 - delete account\n"
           "5 - exit\n? ");

    scanf("%u", &menuChoice); // get user input
    return menuChoice;
}
