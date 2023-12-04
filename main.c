//Including the neccesary header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cJSON.h"

//Defining a sleep function to pause execution of the program for certain seconds
#ifdef _WIN32
#include <windows.h>
#define sleepFor(seconds) Sleep(seconds * 1000)
#define CLEAR "cls"
#else
#include <unistd.h>
#define CLEAR "clear"
#define sleepFor(seconds) sleep(seconds)
#endif


// Enum to represent data types
typedef enum {
    INT_TYPE =1,
    DOUBLE_TYPE =2,
    STRING_TYPE=3,
    DICTIONARY_TYPE =4
} DataType;

// Union to store different data types
typedef union Datavalue{
    int int_val;
    double double_val;
    char* string_val;
    struct Dictionary* dictionary_val;
} DataValue;

// Define a structure for key-value pairs with data type
typedef struct KeyValuePair {
    char* key;
    DataType type;
    DataValue value;
} KeyValuePair;

// Define a structure for nested dictionaries
typedef struct Dictionary {
    KeyValuePair* data;  // array of key-value pairs
    size_t size;
} Dictionary;

// Define a structure for the in-memory database collection
typedef struct Collection{
    char* name;
    KeyValuePair* data;
    size_t size;
} Collection;

// Define a structure for the in-memory database
typedef struct InMemoryDatabase{
    Collection* collections;
    size_t num_collections;
} InMemoryDatabase;

typedef struct Response{
    DataValue value;
    DataType type;
} Response;

const char* getDataType[] = { "INT", "DOUBLE", "STRING" ,"DICTIONARY" };
const DataType getDataTypeE[] = { INT_TYPE, DOUBLE_TYPE, STRING_TYPE ,DICTIONARY_TYPE };

// Function to init a nested dictionary
void initDictionary(Dictionary* dictionary) {
    dictionary->data = NULL;
    dictionary->size = 0;
}

// Function to init the in-memory database
void initDatabase(InMemoryDatabase* db) {
    db->collections = NULL;
    db->num_collections = 0;
}
void initDataValueWithDictionary(DataValue* dataValue) {
    dataValue->dictionary_val = malloc(sizeof(Dictionary));
    initDictionary(dataValue->dictionary_val);
}

// Function to add a collection to the database
void addCollection(InMemoryDatabase* db, const char* name) {
    db->collections = realloc(db->collections, (db->num_collections + 1) * sizeof(Collection));
    db->collections[db->num_collections].name = strdup(name);
    db->collections[db->num_collections].data = NULL;
    db->collections[db->num_collections].size = 0;
    db->num_collections++;
}

// Function to add a key-value pair to a collection in the database
void addToCollection(InMemoryDatabase* db, const char* collectionName, const char* key, DataType type, DataValue value) {
    // Find the collection
    for (size_t i = 0; i < db->num_collections; ++i) {
        if (strcmp(db->collections[i].name, collectionName) == 0) {
            // Check if the key already exists
            for (size_t j = 0; j < db->collections[i].size; ++j) {
                if (strcmp(db->collections[i].data[j].key, key) == 0) {
                    // Update the value if the key already exists
                    db->collections[i].data[j].type = type;
                    if (type == STRING_TYPE) {
                        if (value.string_val != NULL) {
                            // free(db->collections[i].data[j].value.string_val);
                            db->collections[i].data[j].value.string_val = strdup(value.string_val);
                        } else {
                            fprintf(stderr, "Invalid string value\n");
                        }
                        
                       
                        sleepFor(2 );
                    } else if (type == DICTIONARY_TYPE) {
                        // Free existing dictionary and copy the new one
                        free(db->collections[i].data[j].value.dictionary_val);
                        db->collections[i].data[j].value.dictionary_val = malloc(sizeof(Dictionary));
                        memcpy(db->collections[i].data[j].value.dictionary_val, value.dictionary_val, sizeof(Dictionary));
                    } else {
                        db->collections[i].data[j].value = value;
                    }
                    printf("Query completed successfully\n");
                    return;
                }
            }

            // Allocate memory for a new key-value pair
            db->collections[i].data = realloc(db->collections[i].data, (db->collections[i].size + 1) * sizeof(KeyValuePair));
            db->collections[i].data[db->collections[i].size].key = strdup(key);
            db->collections[i].data[db->collections[i].size].type = type;
            if (type == STRING_TYPE) {
                if (value.string_val != NULL) {
                    db->collections[i].data[db->collections[i].size].value.string_val = strdup(value.string_val);
                } else {
                    fprintf(stderr, "Invalid string value\n");
                }
                
            } else if (type == DICTIONARY_TYPE) {
                db->collections[i].data[db->collections[i].size].value.dictionary_val = malloc(sizeof(Dictionary));
                memcpy(db->collections[i].data[db->collections[i].size].value.dictionary_val, value.dictionary_val, sizeof(Dictionary));
            } else {
                db->collections[i].data[db->collections[i].size].value = value;
            }
            db->collections[i].size++;
            return;
        }
    }

    // Collection not found
    fprintf(stderr, "Collection %s not found\n", collectionName);
}

int isCollectionExist(const InMemoryDatabase *db, const char* collectionName){
    for (size_t i = 0; i < db->num_collections; ++i) {
        if (strcmp(db->collections[i].name, collectionName) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to get the value associated with a key from a collection in the database
Response getFromCollection(const InMemoryDatabase* db, const char* collectionName, const char* key) {
    Response notFound;
    notFound.value.string_val = NULL;
    int i;
    if((i = isCollectionExist(db,collectionName)) >= 0){
        for (size_t j = 0; j < db->collections[i].size; ++j) {
                 if (strcmp(db->collections[i].data[j].key, key) == 0) {
                Response response;
                response.type = db->collections[i].data[j].type;
                if (response.type == DICTIONARY_TYPE) {
                    response.value.dictionary_val = db->collections[i].data[j].value.dictionary_val;
                } else {
                    response.value = db->collections[i].data[j].value;
                }
                return response;
            }
            }
            // Key not found in the collection
            fprintf(stderr, "Key %s not found in collection %s\n", key, collectionName);
            return notFound;
    }else{
        fprintf(stderr, "Collection %s not found\n", collectionName);
        return notFound;
    }

    
}

// Function to set a key-value pair in a nested dictionary
void setDictionaryKeyValue(Dictionary* dictionary, const char* key, DataType type, DataValue value) {
    // Check if the key already exists
    for (size_t i = 0; i < dictionary->size; ++i) {
        if (strcmp(dictionary->data[i].key, key) == 0) {
            // Update the value if the key already exists
            dictionary->data[i].type = type;
            if (type == STRING_TYPE) {
                free(dictionary->data[i].value.string_val);
                dictionary->data[i].value.string_val = strdup(value.string_val);
            } else if (type == DICTIONARY_TYPE) {
                // Free existing dictionary and copy the new one
                free(dictionary->data[i].value.dictionary_val);
                dictionary->data[i].value.dictionary_val = malloc(sizeof(Dictionary));
                memcpy(dictionary->data[i].value.dictionary_val, value.dictionary_val, sizeof(Dictionary));
            } else {
                dictionary->data[i].value = value;
            }
            return;
        }
    }

    // Allocate memory for a new key-value pair
    dictionary->data = realloc(dictionary->data, (dictionary->size + 1) * sizeof(KeyValuePair));
    dictionary->data[dictionary->size].key = strdup(key);
    dictionary->data[dictionary->size].type = type;
    if (type == STRING_TYPE) {
        dictionary->data[dictionary->size].value.string_val = strdup(value.string_val);
    } else if (type == DICTIONARY_TYPE) {
        dictionary->data[dictionary->size].value.dictionary_val = malloc(sizeof(Dictionary));
        memcpy(dictionary->data[dictionary->size].value.dictionary_val, value.dictionary_val, sizeof(Dictionary));
    } else {
        dictionary->data[dictionary->size].value = value;
    }
    dictionary->size++;
}

// Function to get all entries in Dictionary
void getAllEntriesInDictionary(const char* collectionName,const char *dictName,const Dictionary* dictionary) {
    
    for (size_t i = 0; i < dictionary->size; ++i) {
        DataType type = dictionary->data[i].type;
        // printf("Key: %s -> Value: ", dictionary->data[i].key);

        switch (type) {
            case INT_TYPE:
                printf("(%s)-> (%s) -> (%s) : %d \n",collectionName,dictName,dictionary->data[i].key, dictionary->data[i].value.int_val);
                break;
            case DOUBLE_TYPE:
                printf("(%s)-> (%s) -> (%s) : %lf \n",collectionName,dictName,dictionary->data[i].key, dictionary->data[i].value.double_val);
                break;
            case STRING_TYPE:
                printf("(%s)-> (%s) -> (%s) : %s \n",collectionName,dictName,dictionary->data[i].key, dictionary->data[i].value.string_val);
                break;
            case DICTIONARY_TYPE:
                // printf("Dictionary\n");
                // If you want to print entries of nested dictionaries, you can recursively call this function here.
                getAllEntriesInDictionary(strdup(collectionName),dictionary->data[i].key,dictionary->data[i].value.dictionary_val);
                break;
            default:
                printf("Unknown Type\n");
                break;
        }
    }
}

// Function to get all entries inside a collection
void getAllEntriesInCollection(const InMemoryDatabase* db, const char* collectionName) {
    for (size_t i = 0; i < db->num_collections; ++i) {
        if (strcmp(db->collections[i].name, collectionName) == 0) {
            printf("Entries in collection %s:\n", collectionName);
            for (size_t j = 0; j < db->collections[i].size; ++j) {
                DataType type = db->collections[i].data[j].type;
               

                switch (type) {
                    case INT_TYPE:
                        // getDataType[type-1]
                        printf("(%s) -- key: %s -> value: %d\n",collectionName,db->collections[i].data[j].key, db->collections[i].data[j].value.int_val);
                        break;
                    case DOUBLE_TYPE:
                  
                        printf("(%s) -- key: %s -> value: %lf\n",collectionName,db->collections[i].data[j].key, db->collections[i].data[j].value.double_val);
                        break;
                    case STRING_TYPE:
                
                        printf("(%s) -- key: %s -> value: %s\n",collectionName,db->collections[i].data[j].key, db->collections[i].data[j].value.string_val);
                        break;
                    case DICTIONARY_TYPE:
                       
                        getAllEntriesInDictionary(strdup(collectionName),db->collections[i].data[j].key,db->collections[i].data[j].value.dictionary_val);
                        break;
                    default:
                        printf("Unknown Type\n");
                        break;
                }
            }
            return;
        }
    }

    // Collection not found
    fprintf(stderr, "Collection %s not found\n", collectionName);
}

// Function to compare key-value pairs based on a specified key
int compareKeyValuePair(const KeyValuePair* a, const KeyValuePair* b, const char* key) {
    const DataValue valueA = a->value;
    const DataValue valueB = b->value;

    // Check if either 'a' or 'b' matches the user entered key
    if (((valueA.string_val && strcmp(a->key, key) == 0) ||
         (valueB.string_val && strcmp(b->key, key) == 0)) &&
        valueA.string_val && valueB.string_val) {
        // If either 'a' or 'b' matches the specified key and both have string values,
        // compare the string values
        return strcmp(valueA.string_val, valueB.string_val);
    } else if (strcmp(((const KeyValuePair*)a)->key, key) == 0) {
        // If 'a' matches the specified key, compare the values
        switch (a->type) {
            case INT_TYPE:
                return valueA.int_val - valueB.int_val;
            case DOUBLE_TYPE:
                return (valueA.double_val > valueB.double_val) - (valueA.double_val < valueB.double_val);
            default:
                
                break;
        }
    } else if (strcmp(((const KeyValuePair*)b)->key, key) == 0) {
        // If 'b' matches the specified key, compare the values
        switch (b->type) {
            case INT_TYPE:
                return valueA.int_val - valueB.int_val;
            case DOUBLE_TYPE:
                return (valueA.double_val > valueB.double_val) - (valueA.double_val < valueB.double_val);
            default:
                
                break;
        }
    }

    // If neither 'a' nor 'b' matches the specified key or they have non-string values,
    // compare based on the specified key
    return strcmp(((const KeyValuePair*)a)->key, ((const KeyValuePair*)b)->key);
}


// Function to free the memory used by a nested dictionary
void freeDictionary(Dictionary* dictionary) {
    for (size_t i = 0; i < dictionary->size; ++i) {
        free(dictionary->data[i].key);
        if (dictionary->data[i].type == STRING_TYPE) {
            free(dictionary->data[i].value.string_val);
        } else if (dictionary->data[i].type == DICTIONARY_TYPE) {
            freeDictionary(dictionary->data[i].value.dictionary_val);
            free(dictionary->data[i].value.dictionary_val);
        }
    }
    free(dictionary->data);
    dictionary->size = 0;
}

// Function to free the memory used by a collection
void freeCollection(Collection* collection) {
    free(collection->name);
    for (size_t i = 0; i < collection->size; ++i) {
        free(collection->data[i].key);
        if (collection->data[i].type == STRING_TYPE) {
            free(collection->data[i].value.string_val);
        } else if (collection->data[i].type == DICTIONARY_TYPE) {
            freeDictionary(collection->data[i].value.dictionary_val);
            free(collection->data[i].value.dictionary_val);
        }
    }
    free(collection->data);
    collection->size = 0;
}

// Function to free the memory used by the entire database
void freeDatabase(InMemoryDatabase* db) {
    for (size_t i = 0; i < db->num_collections; ++i) {
        freeCollection(&db->collections[i]);
    }
    free(db->collections);
    db->num_collections = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parsing The Database:

// Function to identify key-value pairs 
DataValue KeyValue_IdentifyDict(cJSON *root, const char *collectionName, Dictionary *dictionary) {
    cJSON *element = NULL;
    DataValue value1;
    cJSON_ArrayForEach(element, root) {
        printf("Key: %s\n", element->string);

        switch (element->type) {
            case cJSON_Number:
               
                if ((double)element->valueint == element->valuedouble) {
                    value1.int_val = element->valueint;
                    setDictionaryKeyValue(dictionary, strdup(element->string), INT_TYPE, value1);
                    // printf("Value (Number): %d\n", value1.int_val);
                } else {
                    value1.double_val = element->valuedouble;
                    setDictionaryKeyValue(dictionary, strdup(element->string), DOUBLE_TYPE, value1);
                    // printf("Value (Number): %lf\n", value1.double_val);
                }
                break;
            case cJSON_String:
              
                value1.string_val = strdup(element->valuestring);
                setDictionaryKeyValue(dictionary, strdup(element->string), STRING_TYPE, value1);
                printf("Value (String): %s\n", value1.string_val);
                break;
            case cJSON_Object:
              
                value1.dictionary_val = malloc(sizeof(Dictionary));
                initDictionary(value1.dictionary_val);
                KeyValue_IdentifyDict(element, strdup(collectionName), value1.dictionary_val);
                setDictionaryKeyValue(dictionary, strdup(element->string), DICTIONARY_TYPE, value1);
                break;
           
            default:
                // printf("Value (Type %d)\n", element->type);
                break;
        }
    }
    return value1;
}

DataValue KeyValue_Identify(cJSON *root,  InMemoryDatabase *db){
    cJSON *element = NULL;
    int i = 0;
    DataValue value1;
    cJSON_ArrayForEach(element, root) {
            printf("Root: %sKey: %s\n",root->string, element->string);
            // Check the type of the item value
            switch (element->type) {
                case cJSON_Array:
                    break;
                case cJSON_Object:
                 
                    value1.dictionary_val = malloc(sizeof(Dictionary));
                    initDictionary(value1.dictionary_val);
                    KeyValue_IdentifyDict(element, strdup(root->string), value1.dictionary_val);
                    addToCollection(db, strdup(root->string), strdup(element->string), DICTIONARY_TYPE, value1);
                    break;
                    
                    
                case cJSON_Number:
                    if ((double)element->valueint == element->valuedouble){
                        value1.int_val = element->valueint;
                        addToCollection(db, strdup(root->string), strdup(element->string), INT_TYPE, value1);
                    
                        printf("Value (Number): %d\n", value1.int_val );
                    }
                    else{
                        value1.double_val = element->valuedouble;
                        addToCollection(db, strdup(root->string), strdup(element->string), DOUBLE_TYPE, value1);
                        printf("Value (Number): %lf\n", value1.double_val );
                    }
                    break;
                    
                case cJSON_String:
                    value1.string_val = strdup(element->valuestring);
                    addToCollection(db, strdup(root->string), strdup(element->string), STRING_TYPE, value1);
                    printf("%s\n",root->string);
                    printf("Value (String): %s\n", value1.string_val);
                    break;
                    
                // Handle other value types as needed
                default:
                    printf("Value (Type %d)\n", element->type);
                    break;

            }
        }
        return value1;
    }

//Function to identify collections
void Collection_Identify(cJSON *root,  InMemoryDatabase *db){
    cJSON *element = NULL;
    cJSON_ArrayForEach(element, root) {
        if (element->type == cJSON_Object || element->type == cJSON_String || element->type == cJSON_Number || element->type == cJSON_True || element->type == cJSON_False || element->type == cJSON_NULL) {
            //printf("Collection: %s\n", element->string);
            // collections[i].name = element->string;
            char _[20] ;
            strcpy(_ , element->string);
            addCollection(db, strdup(_)); 
        }
        
        KeyValue_Identify(element,db);
    }
}

void displayMenu(int i){
    system(CLEAR);

    printf(""
    "(               (   \n"        
    "  (   )\\ )            )\\ )    (  \n"
    "( )\\ (()/(    (   (  (()/(  ( )\\  \n"
    ")((_) /(_))   )\\  )\\  /(_)) )((_) \n"
    "((_)_ (_))  _ ((_)((_)(_))_ ((_)_  \n"
    "| _ )| |  | | | || __||   \\ | _ ) \n"
    "| _ \\| |__| |_| || _| | |) || _ \\ \n"
    "|___/|____|\\___/ |___||___/ |___/ \n\n\n");


    if (i ==0){
        printf("%s","1. Add Collection\n2. Add Data\n3. Get Data\n4. Update Data\n5. Load Database\n6. Save Database\n7. Exit\n\n");
    }
    else if (i == 2){
        printf("%s","Select datatype of entry:- \n1. INTEGER\n2. DOUBLE\n3. STRING\n4. DICTIONARY\n5. Back to Home\n6. Exit\n\n");
    }
    else if (i == 3){
        printf("%s","Select datatype of entry:- \n1. INTEGER\n2. DOUBLE\n3. STRING\n5. Back to Home\n6. Exit\n\n");
    }
} 

void pressEnterToContinue() {
    printf("Press Enter to continue...");
    fflush(stdout);

     int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int main(){
    InMemoryDatabase myDatabase;
    Collection *collections;
    initDatabase(&myDatabase);

    // 1. Add Collection\n2. Add Data\n3. Get Data\n4. Update Data\n5.
    //  Load Database\n6. Save Database\n7. Exit
    
    while(true){
        displayMenu(0);
        int input;
        scanf("%d", &input);

        //Checking for option 1:
        if (input == 1){ 
            char q[20];
            printf("Enter collection name: ");
            scanf("%s",q);
            if (isCollectionExist(&myDatabase,strdup(q))<0){
                addCollection(&myDatabase,  strdup(q)); 
                printf("\nAdded Successfully. Returning to main menu in 5 seconds.\n");
            }
            else{
                fprintf(stderr, "\n\"%s\" collection, already exist. Returning to main menu in 5 seconds.\n", q);
            }
            sleepFor(5);
            }

        //Checking for option 2:
        // INT_TYPE,
        // DOUBLE_TYPE,
        // STRING_TYPE,
        // DICTIONARY_TYPE
        // 5. Back to Home
        // 6. Exit
        else if(input == 2 || input == 4){
            displayMenu(2);
            int _ ;
            scanf("%d",&_);
            if ( _ == 5 ){
                displayMenu(0);
                continue;
            }
            else if ( _ ==  6){
                return 1;
            }
            
            char collectionName[20];
            printf("Enter collection name: ");
            scanf("%s",collectionName);

            if (isCollectionExist(&myDatabase,strdup(collectionName))>=0){
                char key[20];
                DataValue value1;
                if ( _ ==  DICTIONARY_TYPE){
                    DataValue value1;
                    initDataValueWithDictionary(&value1);
                    DataValue nestedValue1;
                    char id[20];
                    printf("Enter id of dictionary: ");
                    scanf("%s",id);
                    char key[20];
                    printf("Enter key: ");
                    scanf("%s",key);
                    displayMenu(3);
                    int c ;
                    scanf("%d",&c);
                    printf("Enter value: ");
                    switch (c){
                        case 1:
                            scanf("%d",&nestedValue1.int_val);
                            break;
                        case 2:
                            scanf("%lf",&nestedValue1.double_val);
                            break;
                        case 3:
                            {char fd[255];
                            scanf("%s",fd);
                            nestedValue1.string_val = strdup(fd);}
                            break;
                        case 4:
                            displayMenu(0);
                            continue;
                            break;
                        case 5:
                            return 1;
                        default:
                            continue;
                            break;
                    }

                setDictionaryKeyValue(value1.dictionary_val, strdup(key), getDataTypeE[c-1], nestedValue1);
                addToCollection(&myDatabase, strdup(collectionName), strdup(id), DICTIONARY_TYPE, value1);
                pressEnterToContinue();
                continue;
                }
                
                printf("enter key: ");
                scanf("%s",key);

                if(_ == INT_TYPE){
                    printf("Enter value: ");
                    scanf("%d",&value1.int_val);
                    addToCollection(&myDatabase, collectionName, key, INT_TYPE, value1);
                }
                else if ( _ == DOUBLE_TYPE  ){
                    printf("Enter value: ");
                    scanf("%lf",&value1.double_val);
                    addToCollection(&myDatabase, collectionName, key, DOUBLE_TYPE, value1);
                }
                else if ( _ ==  STRING_TYPE){               
                    printf("Enter value: ");
                    char s[255];
                    scanf("%s",s);
                    // printf("%s\n",value1.string_val);
                    value1.string_val = strdup(s);
                    addToCollection(&myDatabase, collectionName, key, STRING_TYPE, value1);
                }
           
            }
            else{
                printf("Collection %s not found.\nReturning to main menu in 5 seconds.\n", collectionName);        
            }
            
            pressEnterToContinue();
        }
        else if(input == 3){
            int _;
            printf("1.Get all from collection\n2. Get from collection\n");
            scanf("%d",&_);
            if(_==1){
                char collectionName[20];
                printf("Enter collection name: ");
                scanf("%s",collectionName);
                if (isCollectionExist(&myDatabase,strdup(collectionName))>=0){
                    getAllEntriesInCollection(&myDatabase,strdup(collectionName));
                }
                else{
                    fprintf(stderr, "Collection %s not found\n", collectionName);
                }
                printf("Press 1 continue.\n\n");
                scanf("%d",&input);
                if (input!=1){
                    break;
                }
                //sleepFor(4 );
                pressEnterToContinue();
            }
            else{
                while(true){
                    char collectionName[20];
                    printf("Enter collection name: ");
                    scanf("%s",collectionName);
                    if(isCollectionExist(&myDatabase,strdup(collectionName))>=0){
                        char key[20];  
                        printf("enter key: ");
                        scanf("%s",key);
                        Response response = getFromCollection(&myDatabase, strdup(collectionName), key);
            
                        if (response.type == STRING_TYPE) {
                            printf("(%s) --> %s : %s\n", collectionName, key, response.value.string_val);
                        } 
                        else if (response.type == DICTIONARY_TYPE) {
                            getAllEntriesInDictionary(strdup(collectionName),key ,response.value.dictionary_val);
                            // printf("(%s) --> %s : Dictionary\n", collectionName, key);
                        }  
                        else if (response.type == DOUBLE_TYPE) {
                        printf("(%s) --> %s : %lf\n", collectionName, key, response.value.double_val);
                        }
                        else{
                        printf("(%s) --> %s : %d\n", collectionName, key, response.value.int_val);
                        }
                    }
                    else{
                        fprintf(stderr, "Collection %s not found\n", collectionName);
                    }   
                    printf("Press 1 to query more or 0 to exit.\n\n");
                    scanf("%d",&input);
                    if (!input){
                        break;
                    }
                }
            }
        }
        else if( input == 5 ){
            char file_name[50];
            printf("\n\n");
            printf("Enter address of JSON File to be loaded: ");
            scanf("%s", file_name);
            printf("\n\n");

            FILE *file = fopen(file_name, "r");
            
            if(!file){
                printf("Error opening file.\n");
                return 1;
            }

            //Getting the File size
            fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            // Read the file content into a buffer
            char *buffer = (char *)malloc(fileSize + 1);
            if (!buffer) {
                fclose(file);
                printf("Memory allocation error.\n");
                return 1;
            }
            fread(buffer, 1, fileSize, file);
            fclose(file);

            //Add a Null character at end
            buffer[fileSize] = '\0';

            // Parse the JSON
            cJSON *json = cJSON_Parse(buffer);
            
            // Free the allocated buffer
            free(buffer); 

            if (!json) {
                printf("Error parsing JSON.\n");
                return 1;
            }

            //Finding out the all the collections and Number of Collections
            Collection_Identify(json ,&myDatabase);
            pressEnterToContinue();
                }

        else if(input == 6){
            for (int i =0; i < myDatabase.num_collections; i++){
            printf("||||||| %s |||||||\n",myDatabase.collections[i].name);
            getAllEntriesInCollection(&myDatabase,strdup(myDatabase.collections[i].name));
            printf("\n--------------------------------------------\n\n");
            }
            sleepFor(20);
        }

        else if(input == 7){
            return 1;
        }
    }

    freeDatabase(&myDatabase);
    return 0;
}
