#include <stdio.h>
#include <stdint.h>

enum KeyType
{
    KEYBOARD = 0,
    CONSUMER = 1,
    MACRO = 2
};

struct key
{
    enum KeyType type;
    uint8_t mod;
    uint8_t ammount;
    char code[10];
    uint8_t last;
};

int main()
{
    printf("reading file\n");
    FILE *file = fopen("flashdata.bin", "rb"); // Open file for reading in binary mode

    if (!file)
    {
        printf("Error opening file\n");
        return 1;
    }

    struct key keys[8];
    int i, j;
    int index = 0;
    int incremental = 12; // Assuming 12 bytes per record
    printf("starting first for\n");
    for (i = 0; i < 8; i++)
    {
        uint8_t type_byte;
        fread(&type_byte, sizeof(uint8_t), 1, file); // Read the type byte

        if (type_byte == 0)
        {
            keys[i].type = KEYBOARD;
            fread(&keys[i].mod, sizeof(uint8_t), 1, file);  // Read the mod byte
            fread(&keys[i].code[0], sizeof(char), 1, file); // Read the code byte
            keys[i].last = 0;
        }
        else if (type_byte == 1)
        {
            printf("\n");
            printf("Before\n");
            printf("index: %d\n", i);
            printf("Type: %X\n", keys[i].type);
            printf("Mod: %X\n", keys[i].mod);
            printf("Ammount: %X\n", keys[i].ammount);
            printf("Code: %X\n", keys[i].code[0]);
            printf("Last: %X\n", keys[i].last);

            keys[i].type = CONSUMER;
            fread(&keys[i].code[0], sizeof(char), 1, file); // Read the code byte
            keys[i].last = 0;

            printf("\n");
            printf("After\n");
            printf("index: %d\n", i);
            printf("Type: %X\n", keys[i].type);
            printf("Mod: %X\n", keys[i].mod);
            printf("Ammount: %X\n", keys[i].ammount);
            printf("Code: %X\n", keys[i].code[0]);
            printf("Last: %X\n", keys[i].last);
            printf("\n");
        }
        else if (type_byte == 2)
        {
            keys[i].type = MACRO;
            fread(&keys[i].ammount, sizeof(uint8_t), 1, file); // Read the amount byte
            for (j = 0; j < keys[i].ammount; j++)
            {
                fread(&keys[i].code[j], sizeof(char), 1, file); // Read the code bytes
            }
            keys[i].last = 0;
        }
        index += incremental;
        fseek(file, index, SEEK_SET); // Move file pointer to next record
    }

    fclose(file);

    // Now keys array contains the data read from the file
    // You can use it as needed

    // next step is to print the data
    for (i = 0; i < 8; i++)
    {
        printf("Index: %d\n", i);
        printf("Type byte: %d\n", keys[i].type);
        if (keys[i].type == KEYBOARD)
        {
            printf("Type: KEYBOARD\n");
            printf("Mod: %X\n", keys[i].mod);
            printf("Code: %X\n", keys[i].code[0]);
            printf("Last: %X\n", keys[i].last);
        }
        else if (keys[i].type == CONSUMER)
        {
            printf("Type: CONSUMER\n");
            printf("Code: %X\n", keys[i].code[0]);
            printf("Last: %X\n", keys[i].last);
        }
        else if (keys[i].type == MACRO)
        {
            printf("Type: MACRO\n");
            printf("Ammount: %X\n", keys[i].ammount);
            for (j = 0; j < keys[i].ammount; j++)
            {
                printf("Code: %X\n", keys[i].code[j]);
            }
            printf("Last: %X\n", keys[i].last);
        }
        printf("\n");
    }

    return 0;
}
