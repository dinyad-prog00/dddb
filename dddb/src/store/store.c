#include "store.h"
#include "../helpers/helpers.h"

char *get_db_path(char db_name[])
{
    char *path = malloc(sizeof(char) * 100);
    if (path == NULL)
    {
        printf("Erreur d'allocation mémoire.\n");
        return NULL;
    }

    sprintf(path, "%s/%s.db", DB_DIR, db_name);
    return path;
}

char *get_collection_path(char db_name[], char collection_name[])
{
    char *path = malloc(sizeof(char) * 100);
    if (path == NULL)
    {
        printf("Erreur d'allocation mémoire.\n");
        return NULL;
    }
    sprintf(path, "%s/%s.db/%s.json", DB_DIR, db_name, collection_name);
    return path;
}

int fileExists(const char *fileName)
{
    return access(fileName, F_OK) != -1;
}

int create_db(char db_name[])
{
    char *path = get_db_path(db_name);

    if (path == NULL)
    {
        printf("Erreur getting db path.\n");
        return 1;
    }

    int status = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status == 0)
    {
        printf("DB \"%s\" created successfully.\n", path);
        free(path);
        return 0;
    }
    else
    {
        printf("Error creating DB %s\n", path);
        free(path);
        return 1;
    }
}

int create_collection(char db_name[], char collection_name[])
{
    char *path = get_collection_path(db_name, collection_name);
    if (path == NULL)
    {
        printf("Erreur getting collection path.\n");
        return 1;
    }
    if (fileExists(path))
    {
        printf("%s already exists\n", path);
        return 1;
    }
    FILE *file = fopen(path, "w");
    fprintf(file, "[]");
    printf("Collection \"%s\" created successfully.\n", path);
    fclose(file);
    free(path);
    return 0;
}

cJSON *get_collection(char db_name[], char collection_name[])
{
    create_collection(db_name, collection_name);
    printf("call get_collection_path\n");
    char *path = get_collection_path(db_name, collection_name);

    printf("fin call collection_path\n");
    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    printf("call fc malloc\n");
    char *fileContent = (char *)malloc(fileSize + 1);

    if (fileContent == NULL)
    {
        printf("Erreur d'allocation mémoire.\n");
        fclose(file);
        return NULL;
    }

    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';

    fclose(file);
    printf("call oooo\n");
    cJSON *collection = cJSON_Parse(fileContent);
    printf("call free\n");
    free(fileContent);
    free(path); // ici
    return collection;
}

int store_collection(cJSON *collection, char *path)
{
    char *s = cJSON_Print(collection);
    printf("%s", s);
    FILE *file = fopen(path, "w");

    if (file == NULL)
    {
        return 1;
    }

    fprintf(file, "%s", s);
    fclose(file);
    return 0;
}

char *generate_uid()
{

    uint32_t timestamp = (uint32_t)time(NULL);
    uint16_t processId = (uint16_t)getpid();

    srand((unsigned int)time(NULL));
    uint32_t counter = rand() % 0xFFFFFF;
    char *uuid = malloc(sizeof(char) * 21);
    sprintf(uuid, "%08x%04x%08x", timestamp, processId, counter);
    return uuid;
}

char *insert_one(char db_name[], char collection_name[], cJSON *data)
{
    create_collection(db_name, collection_name);
    char *path = get_collection_path(db_name, collection_name);
    cJSON *collection = get_collection(db_name, collection_name);

    cJSON_AddStringToObject(data, "_id", generate_uid());
    data->next = NULL;

        if (cJSON_IsObject(data))
    {
        cJSON_AddItemToArray(collection, data);
        store_collection(collection, path);
        return cJSON_Print(data);
    }
    // free(path);
}

cJSON_bool check_attr_value(cJSON *item, char *attr, cJSON *value, cJSON_bool case_sensitive)
{
    if (!cJSON_IsObject(item))
    {
        return 0;
    }

    cJSON *ivalue = cJSON_GetObjectItemCaseSensitive(item, attr);
    if (cJSON_IsObject(value))
    {
        cJSON *in = cJSON_GetObjectItemCaseSensitive(value, "$in");

        if (cJSON_IsArray(in))
        {
            return in_array(in, ivalue);
        }
    }
    return cJSON_Compare(ivalue, value, case_sensitive);
}

cJSON_bool pass_filter(cJSON *item, cJSON *filter)
{
    cJSON *it;
    if (cJSON_IsObject(filter))
    {
        if (cJSON_GetArraySize(filter) == 0)
        {
            return 1;
        }
        cJSON_ArrayForEach(it, filter)
        {
            if (!check_attr_value(item, it->string, it, 1))
            {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

cJSON *find_by_unique_key(char db_name[], char collection_name[], char *key_name, cJSON *key_value)
{
    cJSON *collection = get_collection(db_name, collection_name);
    cJSON *item;

    cJSON_ArrayForEach(item, collection)
    {

        if (check_attr_value(item, key_name, key_value, 1))
        {
            return item;
        }
    }
    return NULL;
}

char *find_by_id(char db_name[], char collection_name[], cJSON *id)
{
    cJSON *collection = get_collection(db_name, collection_name);

    cJSON *item = find_by_unique_key(db_name, collection_name, "_id", id);
    return cJSON_Print(item);
}

char *find(char db_name[], char collection_name[], cJSON *single, cJSON *filter, cJSON *options)
{
    printf("call get_collection\n");
    cJSON *collection = get_collection(db_name, collection_name);
    printf("fin call get_collection\n");
    cJSON *item;

    if (cJSON_Compare(single, cJSON_CreateTrue(), 1))
    {
        cJSON_ArrayForEach(item, collection)
        {
            if (pass_filter(item, filter))
            {
                cJSON *populate = cJSON_GetObjectItemCaseSensitive(options, "populate");
                if (populate != NULL)
                {
                    make_all_references(db_name, item, populate);
                }

                char *str = cJSON_Print(item);
                cJSON_Delete(collection);
                return str;
            }
        }
        // cJSON_Delete(collection);
        return cJSON_Print(cJSON_CreateNull());
    }

    else
    {
        cJSON *result = cJSON_CreateArray();
        cJSON_ArrayForEach(item, collection)
        {
            if (pass_filter(item, filter))
            {
                cJSON_AddItemToArray(result, cJSON_Parse(cJSON_Print(item)));
            }
        }

        cJSON *populate = cJSON_GetObjectItemCaseSensitive(options, "populate");
        if (populate != NULL)
        {
            cJSON_ArrayForEach(item, result)
            {
                make_all_references(db_name, item, populate);
            }
        }

        char *str = cJSON_Print(result);
        // cJSON_Delete(collection);
        // cJSON_Delete(result);
        return str;
    }
}

char *get_all(char db_name[], char collection_name[], cJSON *options)
{
    printf("aaaaaaaaaaaaaaaaaaaaaaaa");
    cJSON *collection = get_collection(db_name, collection_name);
    cJSON *item;

    cJSON *populate = cJSON_GetObjectItemCaseSensitive(options, "populate");
    printf("aaaaaaaaaaaaaaaaaaaaaaaa");
    if (populate != NULL)

    {
        printf("aaaaaaaaaaaaaaaaaaaaaaaa");
        cJSON_ArrayForEach(item, collection)
        {
            make_all_references(db_name, item, populate);
        }
    }

    char *str = cJSON_Print(collection);

    // cJSON_Delete(collection);
    return str;
}

char *delete(char db_name[], char collection_name[], cJSON *single, cJSON *filter)
{
    char *path = get_collection_path(db_name, collection_name);
    cJSON *collection = get_collection(db_name, collection_name);
    cJSON *tmp = cJSON_Parse(cJSON_Print(collection));
    cJSON *item;
    int nb = 0, i = 0;
    cJSON *rst = cJSON_CreateObject();
    if (cJSON_Compare(single, cJSON_CreateTrue(), 1))
    {
        cJSON_ArrayForEach(item, collection)
        {
            if (pass_filter(item, filter))
            {
                cJSON_DetachItemViaPointer(collection, item);
                store_collection(collection, path);
                cJSON_AddNumberToObject(rst, "deleted", 1);
                return cJSON_Print(rst);
            }
        }

        cJSON_AddNumberToObject(rst, "deleted", 0);
        return cJSON_Print(rst);
    }

    else
    {
        cJSON *result = cJSON_CreateArray();
        cJSON_ArrayForEach(item, collection)
        {
            if (pass_filter(item, filter))
            {
                cJSON_DeleteItemFromArray(tmp, i - nb);
                nb++;
            }
            i++;
        }

        store_collection(tmp, path);
        cJSON_AddNumberToObject(rst, "deleted", nb);
        return cJSON_Print(rst);
    }
}

char *update(char db_name[], char collection_name[], cJSON *single, cJSON *filter, cJSON *data)
{

    char *path = get_collection_path(db_name, collection_name);
    cJSON *collection = get_collection(db_name, collection_name);
    cJSON *item, *it;
    int nb = 0, i = 0;
    cJSON *rst = cJSON_CreateObject();
    if (cJSON_Compare(single, cJSON_CreateTrue(), 1))
    {
        cJSON_ArrayForEach(item, collection)
        {
            if (pass_filter(item, filter))
            {
                cJSON_ArrayForEach(it, data)
                {
                    if (strcmp(it->string, "_id"))
                    {
                        if (cJSON_HasObjectItem(item, it->string))
                        {

                            cJSON_DeleteItemFromObject(item, it->string);
                            cJSON_AddItemToObject(item, it->string, cJSON_Parse(cJSON_Print(it)));
                        }
                        else
                        {
                            cJSON_AddItemToObject(item, it->string, cJSON_Parse(cJSON_Print(it)));
                        }
                    }
                }

                cJSON_ReplaceItemInArray(collection, i, item);
                store_collection(collection, path);
                return cJSON_Print(item);
            }
            i++;
        }

        cJSON_AddNumberToObject(rst, "updated", 0);
        return cJSON_Print(rst);
    }

    else
    {

        cJSON *result = cJSON_CreateArray();
        cJSON_ArrayForEach(item, collection)
        {
            if (pass_filter(item, filter))
            {

                cJSON_ArrayForEach(it, data)
                {
                    if (strcmp(it->string, "_id"))
                    {
                        if (cJSON_HasObjectItem(item, it->string))
                        {
                            cJSON_DeleteItemFromObject(item, it->string);
                            cJSON_AddItemToObject(item, it->string, cJSON_Parse(cJSON_Print(it)));
                        }
                        else
                        {
                            cJSON_AddItemToObject(item, it->string, cJSON_Parse(cJSON_Print(it)));
                        }
                    }
                }
                cJSON_ReplaceItemInArray(collection, i, item);
                nb++;
            }
            i++;
        }

        store_collection(collection, path);
        cJSON_AddNumberToObject(rst, "updated", nb);
        return cJSON_Print(rst);
    }
}

RefProps *make_ref_props(char *attr)
{
    RefProps *prop = malloc(sizeof(RefProps));

    if (prop == NULL)
    {
        printf("Error d'allocation mémoire");
        return NULL;
    }

    prop->type = -1;
    char *refStart;
    if (!strncmp(attr, "_ref", 4))
    {
        refStart = attr + 6;
        prop->type = SINGLE_REF;
    }
    else if (!strncmp(attr, "_refs", 5))
    {
        refStart = attr + 7;
        prop->type = MANY_REF;
    }

    if (prop->type != -1)
    {
        char *refEnd = strstr(refStart, "__");

        if (refEnd != NULL)
        {
            strncpy(prop->ref, refStart, refEnd - refStart);
            prop->ref[refEnd - refStart] = '\0';
        }
        else
        {
            return NULL;
        }

        char *name = refEnd + 2;

        if (strlen(name) != 0)
        {
            strcpy(prop->name, name);
            return prop;
        }
        else
        {
            return NULL;
        }
    }

    return NULL;
}

void make_reference(char db_name[], cJSON *item, char *attr, RefProps *prop)
{

    cJSON *attr_value = cJSON_GetObjectItemCaseSensitive(item, attr);

    if (cJSON_IsString(attr_value) && prop->type == SINGLE_REF)
    {

        cJSON *filter = cJSON_CreateObject();
        cJSON_AddItemToObject(filter, "_id", cJSON_Parse(cJSON_Print(attr_value)));

        cJSON *ref = cJSON_Parse(find(db_name, prop->ref, cJSON_CreateTrue(), filter, NULL));
        cJSON_AddItemToObject(item, prop->name, ref);
        // cJSON_DeleteItemFromObject(item, attr);
        // printf("af i : %s\n", cJSON_Print(item));
        printf("azertyu========================================1");
    }
    else if (cJSON_IsArray(attr_value) && prop->type == MANY_REF)
    {
        printf("azertyu========================================");
        cJSON *filter = cJSON_CreateObject();
        cJSON *in = cJSON_CreateObject();
        cJSON_AddItemToObject(in, "$in", cJSON_Parse(cJSON_Print(attr_value)));
        cJSON_AddItemToObject(filter, "_id", in);

        printf("azertyu========================================");

        cJSON *ref = cJSON_Parse(find(db_name, prop->ref, cJSON_CreateFalse(), filter, NULL));
        cJSON_AddItemToObject(item, prop->name, ref);
    }
    else
    {
        printf("azertyu======================================== else");
    }
}

void make_all_references(char db_name[], cJSON *item, cJSON *populate)
{
    cJSON *it;
    // char ** ref_attrs = malloc(sizeof(char *));
    // int n = 0;
    cJSON_ArrayForEach(it, item)
    {
        if (in_array(populate, cJSON_CreateString(it->string)))
        {
            RefProps *prop = make_ref_props(it->string);
            if (prop != NULL)
            {
                // printf("oui=%s\n", it->string);
                // printf("item=%s\n", cJSON_Print(item));
                make_reference(db_name, item, it->string, prop);
                // printf("item fin=%s\n", cJSON_Print(item));
                //  strcpy(ref_attrs[n], item->string);
                //  printf("azerty==========");
                //  n++;
            }
        }
    }
    // for (int i = 0; i < n; i++)
    // {
    //     cJSON_DeleteItemFromObject(item, ref_attrs[i]);
    // }
}

int in_array(cJSON *array, cJSON *item)
{
    cJSON *it;
    cJSON_ArrayForEach(it, array)
    {
        if (cJSON_Compare(it, item, 1))
        {
            return 1;
        }
    }
    return 0;
}

// void clear_ref_attr(cJSON *item)
// {
//     char *ref_attrs[10];
//     int n = 0;
//     cJSON_ArrayForEach(it, item)
//     {
//         RefProps *prop = make_ref_props(it->string);
//         if (prop != NULL)
//         {
//             strcpy(ref_attrs[n++], item->string);
//         }
//     }

//     for (int i = 0; i < n; i++)
//     {
//         cJSON_DeleteItemFromObject(item, ref_attrs[i]);
//     }
// }

int compare_by_attr(cJSON *a, cJSON *b, char *attr)
{
    cJSON *a_attr = cJSON_GetObjectItemCaseSensitive(a, attr);
    cJSON *b_attr = cJSON_GetObjectItemCaseSensitive(b, attr);
    if (a_attr == NULL && b_attr == NULL)
    {
        return 0;
    }
    else if (a_attr == NULL)
    {
        return -1;
    }
    else if (b_attr == NULL)
    {
        return 1;
    }

    if (a_attr->type == cJSON_Number && b_attr->type == cJSON_Number)
    {
        return a_attr->valuedouble - b_attr->valuedouble;
    }

    if (a_attr->type == cJSON_String && b_attr->type == cJSON_String)
    {
        return strcmp(a_attr->valuestring, b_attr->valuestring);
    }

    if (cJSON_Compare(a_attr, b_attr, 1))
    {
        return 0;
    }

    return 1;
}

cJSON *sort(cJSON *array, char *by, int desc)
{
    cJSON **tmp = malloc(sizeof(cJSON *) * cJSON_GetArraySize(array)), *it;
    int i;
    cJSON_ArrayForEach(it, array)
    {
        tmp[i] = cJSON_Parse(cJSON_Print(it));
        i++;
    }

    // qsort(tmp, i, sizeof(cJSON *), compare_by_attr,);
}