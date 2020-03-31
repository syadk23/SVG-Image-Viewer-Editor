/* Syad Khwajazada 1046304 */
#include "SVGParser.h"
#include <ctype.h>
#include <math.h>

#define pi 3.14159

/* Initialize all the lists for each element */
void initLists(SVGimage *img)
{
    img->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    img->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    img->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    img->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    img->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
}

void groupInit(Group *grp)
{
    grp->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    grp->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    grp->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    grp->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    grp->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
}

Path* addPathNode(xmlNode *node, SVGimage *img)
{
    xmlAttr *attr;
    Path *path_node;
    path_node = malloc(sizeof(Path));
    path_node->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    for (attr = node->properties; attr; attr = attr->next)
    {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        if (strcmp(attrName, "d") == 0)
        {
            path_node->data = malloc(sizeof(char) * (strlen(cont) + 1));
            strcpy(path_node->data, cont);
        }
        else
        {
            Attribute *attribute_node = malloc(sizeof(Attribute));
            attribute_node->name = malloc(sizeof(char) * (strlen(attrName) + 1));
            attribute_node->value = malloc(sizeof(char) * (strlen(cont) + 1));
            strcpy(attribute_node->name, attrName);
            strcpy(attribute_node->value, cont);
            insertBack(path_node->otherAttributes, attribute_node);
        }
    }
    return path_node;
}

Circle* addCircleNode(xmlNode *node, SVGimage *img)
{
    xmlAttr *attr;
    Circle *circle_node;
    circle_node = malloc(sizeof(Circle));
    circle_node->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    
    char *units;
    circle_node->cx = 0;
    circle_node->cy = 0;
    circle_node->r = 0;
    strcpy(circle_node->units, "");

    for (attr = node->properties; attr; attr = attr->next)
    {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        if (strcmp(attrName, "cx") == 0)
        {
            circle_node->cx = atof(cont);
        }
        else if (strcmp(attrName, "cy") == 0)
        {
            circle_node->cy = atof(cont);
        }
        else if (strcmp(attrName, "r") == 0)
        {
            circle_node->r = strtof(cont, &units);
            strcpy(circle_node->units, units);
        }
        else
        {
            Attribute *attribute_node = malloc(sizeof(Attribute));
            attribute_node->name = malloc(sizeof(char) * (strlen(attrName) + 1));
            attribute_node->value = malloc(sizeof(char) * (strlen(cont) + 1));
            strcpy(attribute_node->name, attrName);
            strcpy(attribute_node->value, cont);
            insertBack(circle_node->otherAttributes, attribute_node);
        }
    }
    return circle_node;
}

Rectangle* addRectangleNode(xmlNode *node, SVGimage *img)
{
    xmlAttr *attr;
    Rectangle *rectangle_node;
    rectangle_node = malloc(sizeof(Rectangle));
    rectangle_node->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    
    char *units;
    rectangle_node->x = 0;
    rectangle_node->y = 0;
    rectangle_node->width = 0;
    rectangle_node->height = 0;
    strcpy(rectangle_node->units, "");

    for (attr = node->properties; attr; attr = attr->next)
    {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        if (strcmp(attrName, "x") == 0)
        {
            rectangle_node->x = atof(cont);
        }
        else if (strcmp(attrName, "y") == 0)
        {
            rectangle_node->y = atof(cont);
        }
        else if (strcmp(attrName, "width") == 0)
        {
           rectangle_node->width = strtof(cont, &units);
           strcpy(rectangle_node->units, units);
        }
        else if (strcmp(attrName, "height") == 0)
        {
            rectangle_node->height = strtof(cont, &units);
            strcpy(rectangle_node->units, units);
        }
        else
        {
            Attribute *attribute_node = malloc(sizeof(Attribute));
            attribute_node->name = malloc(sizeof(char) * (strlen(attrName) + 1));
            attribute_node->value = malloc(sizeof(char) * (strlen(cont) + 1));
            strcpy(attribute_node->name, attrName);
            strcpy(attribute_node->value, cont);
            insertBack(rectangle_node->otherAttributes, attribute_node);
        }
    }
    return rectangle_node;
}

Attribute* addAttributeNode(xmlAttr *attr, SVGimage *img)
{
    Attribute *attribute_node = malloc(sizeof(Attribute));
    xmlNode *value = attr->children;
    char *attrName = (char *)attr->name;
    char *cont = (char *)(value->content);
    attribute_node->name = malloc(sizeof(char) * (strlen(attrName) + 1));
    attribute_node->value = malloc(sizeof(char) * (strlen(cont) + 1));
    strcpy(attribute_node->name, attrName);
    strcpy(attribute_node->value, cont);
    return attribute_node;
}

Group* addGroupNode(xmlNode *node, SVGimage *img)
{
    xmlNode *cur_node = NULL;
    xmlAttr *attr = NULL;
    Group *group_node;
    group_node = malloc(sizeof(Group));
    groupInit(group_node);

    for (attr = node->parent->properties; attr; attr = attr->next)
    {
        Attribute *group_attribute_node = addAttributeNode(attr, img);
        insertBack(group_node->otherAttributes, group_attribute_node);
    }

    for (cur_node = node; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if (strcmp((char *)cur_node->name, "path") == 0)
            {
                Path *group_path_node = addPathNode(cur_node, img);
                insertBack(group_node->paths, group_path_node);
            }
            else if (strcmp((char *)cur_node->name, "circle") == 0)
            {
                Circle *group_circle_node = addCircleNode(cur_node, img);
                insertBack(group_node->circles, group_circle_node);
            }
            else if (strcmp((char *)cur_node->name, "rect") == 0)
            {
                Rectangle *group_rectangle_node = addRectangleNode(cur_node, img);
                insertBack(group_node->rectangles, group_rectangle_node);
            }
            else if (strcmp((char *)cur_node->name, "g") == 0 && cur_node != node)
            {
                Group *group_group_node = addGroupNode(cur_node->children, img);
                insertBack(group_node->groups, group_group_node);
            }
        }
    }
    return group_node;
}

void parseElements(xmlNode *a_node, SVGimage *img)
{
    xmlNode *cur_node = NULL;
    xmlAttr *attr = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {       
            if (strcmp((char *)cur_node->name, "svg") == 0)
            {
                for (attr = a_node->properties; attr; attr = attr->next)
                {
                    Attribute *attribute_node = addAttributeNode(attr, img);
                    insertBack(img->otherAttributes, attribute_node);
                }
            }     

            if (strcmp((char *)cur_node->name, "title") == 0)
            {
                strncpy(img->title, (char *)cur_node->children->content, 255);
                if (img->title[strlen(img->title)] != '\0')
                    strcat(img->title, "\0");
            }

            if (strcmp((char *)cur_node->name, "desc") == 0)
            {
                strncpy(img->description, (char *)cur_node->children->content, 255);
                if (img->description[strlen(img->description)] != '\0')
                    strcat(img->description, "\0");
            }

            if (strcmp((char *)cur_node->name, "path") == 0 && strcmp((char *)cur_node->parent->name, "g") != 0)
            {
                Path *path_node = addPathNode(cur_node, img);
                insertBack(img->paths, path_node);
            }
            else if (strcmp((char *)cur_node->name, "circle") == 0 && strcmp((char *)cur_node->parent->name, "g") != 0)
            {
                Circle *circle_node = addCircleNode(cur_node, img);
                insertBack(img->circles, circle_node);
            }
            else if (strcmp((char *)cur_node->name, "rect") == 0 && strcmp((char *)cur_node->parent->name, "g") != 0)
            {
               Rectangle *rectangle_node = addRectangleNode(cur_node, img);
               insertBack(img->rectangles, rectangle_node);
            }
            else if (strcmp((char *)cur_node->name, "g") == 0 && strcmp((char *)cur_node->parent->name, "g") != 0)
            {
                Group *group_node = addGroupNode(cur_node->children, img);
                insertBack(img->groups, group_node);
            }
        }
        parseElements(cur_node->children, img);
    }
}

SVGimage* createSVGimage(char* fileName)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    SVGimage *img = NULL;

    LIBXML_TEST_VERSION;

    // Read in the xml file into a variable
    doc = xmlReadFile(fileName, NULL, 0);

    if (!doc)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    img = malloc(sizeof(SVGimage));

    root_element = xmlDocGetRootElement(doc);
    initLists(img);
    strncpy(img->namespace, (char *)root_element->nsDef->href, 255);
    if (img->namespace[strlen(img->namespace)] != '\0')
        strcat(img->namespace, "\0");

    strcpy(img->title, "");
    strcpy(img->description, "");
    parseElements(root_element, img);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return img;
}

char* SVGimageToString(SVGimage* img)
{
    char *string = malloc(sizeof(char) * 10);
    strcpy(string, "");
    return string;
}

void deleteSVGimage(SVGimage* img)
{
    if (img)
    {
        if (img->paths)
            freeList(img->paths);
        if (img->rectangles)
            freeList(img->rectangles);
        if (img->circles)
            freeList(img->circles);
        if (img->groups)
            freeList(img->groups);
        if (img->otherAttributes)
            freeList(img->otherAttributes);
        free(img);
    }
}

void deleteAttribute(void* data)
{
    if (!data)
        return;
    Attribute *temp = (Attribute *)data;
    free(temp->name);
    free(temp->value);
    free(temp);
}

char* attributeToString(void* data)
{
    char *string = malloc(sizeof(char) * 10);
    strcpy(string, "");
    return string;
}

int compareAttributes(const void *first, const void *second)
{
    return 0;
}

void deleteGroup(void* data)
{
    if (!data)
        return;
    Group *temp = (Group *)data;
    freeList(temp->circles);
    freeList(temp->rectangles);
    freeList(temp->paths);
    freeList(temp->groups);
    freeList(temp->otherAttributes);
    free(temp);
}

char* groupToString(void* data)
{
    char *string = malloc(sizeof(char) * 10);
    strcpy(string, "");
    return string;
}

int compareGroups(const void *first, const void *second)
{
    return 0;
}

void deleteRectangle(void* data)
{
    if (!data)
        return;
    Rectangle *temp = (Rectangle *)data;
    freeList(temp->otherAttributes);
    free(temp);
}

char* rectangleToString(void* data)
{
    char *string = malloc(sizeof(char) * 10);
    strcpy(string, "");
    return string;
}

int compareRectangles(const void *first, const void *second)
{
    return 0;
}

void deleteCircle(void* data)
{
    if (!data)
        return;
    Circle *temp = (Circle *)data;
    freeList(temp->otherAttributes);
    free(temp);
}

char* circleToString(void* data)
{
    char *string = malloc(sizeof(char) * 10);
    strcpy(string, "");
    return string;
}

int compareCircles(const void *first, const void *second)
{
    return 0;
}

void deletePath(void* data)
{
    if (!data)
        return;
    Path *temp = (Path *)data;
    free(temp->data);
    freeList(temp->otherAttributes);
    free(temp);
}

char* pathToString(void* data)
{
    char *string = malloc(sizeof(char) * 10);
    strcpy(string, "");
    return string;
}

int comparePaths(const void *first, const void *second)
{
    return 0;
}

void deletePlaceholder (void* data)
{
    return;
}

void groupRectFinder(void *elem, List *rectList)
{
    Group *temp = (Group *)elem;
    ListIterator iter = createIterator(temp->rectangles);
    ListIterator groupIter = createIterator(temp->groups);

    while ((elem = nextElement(&iter)) != NULL)
    {
        insertBack(rectList, elem);
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        groupRectFinder(elem, rectList);
    }
}

void groupCircFinder(void *elem, List *circleList)
{
    Group *temp = (Group *)elem;
    ListIterator iter = createIterator(temp->circles);
    ListIterator groupIter = createIterator(temp->groups);

    while ((elem = nextElement(&iter)) != NULL)
    {
        insertBack(circleList, elem);
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        groupCircFinder(elem, circleList);
    }
}

void groupPathFinder(void *elem, List *pathList)
{
    Group *temp = (Group *)elem;
    ListIterator iter = createIterator(temp->paths);
    ListIterator groupIter = createIterator(temp->groups);

    while ((elem = nextElement(&iter)) != NULL)
    {
        insertBack(pathList, elem);
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        groupPathFinder(elem, pathList);
    }
}

void groupGroupFinder(void *elem, List *groupList)
{
    Group *temp = (Group *)elem;
    ListIterator iter = createIterator(temp->groups);

    while ((elem = nextElement(&iter)) != NULL)
    {
        insertBack(groupList, elem);
        groupGroupFinder(elem, groupList);
    }
}

List* getRects(SVGimage* img)
{
    if (!img)
        return NULL;

    List *rectList = initializeList(rectangleToString, deletePlaceholder, compareRectangles);
    ListIterator iter = createIterator(img->rectangles);
    ListIterator groupIter = createIterator(img->groups);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        insertBack(rectList, elem);
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        groupRectFinder(elem, rectList);
    }
    return rectList;
}

List* getCircles(SVGimage* img)
{
    if (!img)
        return NULL;

    List *circleList = initializeList(circleToString, deletePlaceholder, compareCircles);
    ListIterator iter = createIterator(img->circles);
    ListIterator groupIter = createIterator(img->groups);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        insertBack(circleList, elem);
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        groupCircFinder(elem, circleList);
    }
    return circleList;
}

List* getPaths(SVGimage* img)
{
    if (!img)
        return NULL;

    List *pathList = initializeList(pathToString, deletePlaceholder, comparePaths);
    ListIterator iter = createIterator(img->paths);
    ListIterator groupIter = createIterator(img->groups);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        insertBack(pathList, elem);
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        groupPathFinder(elem, pathList);
    }
    return pathList;
}

List* getGroups(SVGimage* img)
{
    if (!img)
        return NULL;

    List *groupList = initializeList(groupToString, deletePlaceholder, compareGroups);
    ListIterator iter = createIterator(img->groups);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        insertBack(groupList, elem);
        groupGroupFinder(elem, groupList);
    }
    return groupList;
}

void findRectsWithArea(void *elem, int *counter, float area)
{
    float calcArea;
    Group *temp = (Group *)elem;
    ListIterator iter = createIterator(temp->rectangles);
    ListIterator groupIter = createIterator(temp->groups);

    while ((elem = nextElement(&iter)) != NULL)
    {
        Rectangle *temp = (Rectangle *)elem;
        calcArea = ceil(temp->width * temp->height);
        if (calcArea == area)
        {
            (*counter)++;
        }
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        findRectsWithArea(elem, counter, area);
    }
}

void findCirclesWithArea(void *elem, int *counter, float area)
{
    float calcArea;
    Group *temp = (Group *)elem;
    ListIterator iter = createIterator(temp->circles);
    ListIterator groupIter = createIterator(temp->groups);

    while ((elem = nextElement(&iter)) != NULL)
    {
        Circle *temp = (Circle *)elem;
        calcArea = ceil(pi * temp->r * temp->r);
        if (calcArea == area)
        {
            (*counter)++;
        }
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        findCirclesWithArea(elem, counter, area);
    }
}

void findPathsWithData(void *elem, int *counter, char *data)
{
    Group *temp = (Group *)elem;
    ListIterator iter = createIterator(temp->paths);
    ListIterator groupIter = createIterator(temp->groups);

    while ((elem = nextElement(&iter)) != NULL)
    {
        Path *temp = (Path *)elem;
        if (strcmp(data, temp->data) == 0)
        {
            (*counter)++;
        }
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        findPathsWithData(elem, counter, data);
    }
}

void findGroupsWithLen(void *elem, int *counter, int len)
{
    Group *temp = (Group *)elem;
    if (temp->rectangles->length + temp->circles->length + temp->paths->length + temp->groups->length == len)
    {
        (*counter)++;
    }
}

int numRectsWithArea(SVGimage* img, float area)
{
    if (!img || !area)
        return 0;

    int counter = 0;
    float calcArea;
    ListIterator iter = createIterator(img->rectangles);
    ListIterator groupIter = createIterator(img->groups);
    void *elem;

    area = ceil(area);

    while ((elem = nextElement(&iter)) != NULL)
    {
        Rectangle *temp = (Rectangle *)elem;
        calcArea = ceil(temp->width * temp->height);
        if (area == calcArea)
        {
            counter++;
        }
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        findRectsWithArea(elem, &counter, area);
    }
    return counter;
}

int numCirclesWithArea(SVGimage* img, float area)
{
    if (!img || !area)
        return 0;

    int counter = 0;
    float calcArea;
    ListIterator iter = createIterator(img->circles);
    ListIterator groupIter = createIterator(img->groups);
    void *elem;

    area = ceil(area);

    while ((elem = nextElement(&iter)) != NULL)
    {
        Circle *temp = (Circle *)elem;
        calcArea = ceil(pi * temp->r * temp->r);
        if (area == calcArea)
        {
            counter++;
        }
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        findCirclesWithArea(elem, &counter, area);
    }
    return counter;
}

int numPathsWithdata(SVGimage* img, char* data)
{
    if (!img || !data)
        return 0;

    int counter = 0;
    ListIterator iter = createIterator(img->paths);
    ListIterator groupIter = createIterator(img->groups);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        Path *temp = (Path *)elem;
        if (strcmp(data, temp->data) == 0)
        {
            counter++;
        }
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        findPathsWithData(elem, &counter, data);
    }
    return counter;
}

int numGroupsWithLen(SVGimage* img, int len)
{
    if (!img || !len)
        return 0;

    int counter = 0;
    ListIterator iter = createIterator(img->groups);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        findGroupsWithLen(elem, &counter, len);
    }
    return counter;
}

int numAttr(SVGimage* img)
{
    if (!img)
        return 0;

    int counter = 0;
    void *elem;

    counter += img->otherAttributes->length;

    List *rectList = getRects(img);
    List *circleList = getCircles(img);
    List *pathList = getPaths(img);
    List *groupList = getGroups(img);

    Rectangle *temp;
    Circle *temp2;
    Path *temp3;
    Group *temp4;

    ListIterator rectIter = createIterator(rectList);
    ListIterator circleIter = createIterator(circleList);
    ListIterator pathIter = createIterator(pathList);
    ListIterator groupIter = createIterator(groupList);

    while ((elem = nextElement(&rectIter)) != NULL)
    {
        temp = (Rectangle *)elem;
        counter += temp->otherAttributes->length;
    }

    while ((elem = nextElement(&circleIter)) != NULL)
    {
        temp2 = (Circle *)elem;
        counter += temp2->otherAttributes->length;
    }

    while ((elem = nextElement(&pathIter)) != NULL)
    {
        temp3 = (Path *)elem;
        counter += temp3->otherAttributes->length;
    }

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        temp4 = (Group *)elem;
        counter += temp4->otherAttributes->length;
    }
    freeList(rectList);
    freeList(circleList);
    freeList(pathList);
    freeList(groupList);
    return counter;
}

void groupRectTree(void *elem, xmlNodePtr outerNode)
{
    Group *temp = (Group *)elem;
    xmlNodePtr node = NULL;
    ListIterator iter = createIterator(temp->rectangles);
    void *attrElem;
    char buffer[255] = "";

    while ((elem = nextElement(&iter)) != NULL)
    {
        node = xmlNewChild(outerNode, NULL, BAD_CAST "rect", NULL);
        Rectangle *temp2 = (Rectangle *)elem;

        snprintf(buffer, 25, "%f", temp2->x);
        xmlNewProp(node, BAD_CAST "x", BAD_CAST buffer);
        snprintf(buffer, 25, "%f", temp2->y);
        xmlNewProp(node, BAD_CAST "y", BAD_CAST buffer);
        snprintf(buffer, 25, "%f", temp2->width);
        xmlNewProp(node, BAD_CAST "width", BAD_CAST buffer);
        snprintf(buffer, 25, "%f", temp2->height);
        xmlNewProp(node, BAD_CAST "height", BAD_CAST buffer);

        ListIterator rectAttrIter = createIterator(temp2->otherAttributes);
        while ((attrElem = nextElement(&rectAttrIter)) != NULL)
        {
            Attribute *temp2 = (Attribute *)attrElem;
            xmlNewProp(node, BAD_CAST temp2->name, BAD_CAST temp2->value);
        }
        xmlAddChild(outerNode, node);
    }
}

void groupCircleTree(void *elem, xmlNodePtr outerNode)
{
    Group *temp = (Group *)elem;
    xmlNodePtr node = NULL;
    ListIterator iter = createIterator(temp->circles);
    void *attrElem;
    char buffer[255] = "";

    while ((elem = nextElement(&iter)) != NULL)
    {
        node = xmlNewChild(outerNode, NULL, BAD_CAST "circle", NULL);
        Circle *temp2 = (Circle *)elem;

        snprintf(buffer, 25, "%f", temp2->cx);
        xmlNewProp(node, BAD_CAST "cx", BAD_CAST buffer);
        snprintf(buffer, 25, "%f", temp2->cy);
        xmlNewProp(node, BAD_CAST "cy", BAD_CAST buffer);
        snprintf(buffer, 25, "%f", temp2->r);
        xmlNewProp(node, BAD_CAST "r", BAD_CAST buffer);

        ListIterator circleAttrIter = createIterator(temp2->otherAttributes);
        while ((attrElem = nextElement(&circleAttrIter)) != NULL)
        {
            Attribute *temp2 = (Attribute *)attrElem;
            xmlNewProp(node, BAD_CAST temp2->name, BAD_CAST temp2->value);
        }
        xmlAddChild(outerNode, node);
    }
}

void groupPathTree(void *elem, xmlNodePtr outerNode)
{
    Group *temp = (Group *)elem;
    xmlNodePtr node = NULL;
    ListIterator iter = createIterator(temp->paths);
    void *attrElem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        node = xmlNewChild(outerNode, NULL, BAD_CAST "path", NULL);
        Path *temp2 = (Path *)elem;

        xmlNewProp(node, BAD_CAST "d", BAD_CAST temp2->data);

        ListIterator pathAttrIter = createIterator(temp2->otherAttributes);
        while ((attrElem = nextElement(&pathAttrIter)) != NULL)
        {
            Attribute *temp2 = (Attribute *)attrElem;
            xmlNewProp(node, BAD_CAST temp2->name, BAD_CAST temp2->value);
        }
        xmlAddChild(outerNode, node);
    }
}

void groupGroupTree(void *elem, xmlNodePtr outerNode)
{
    Group *temp = (Group *)elem;
    xmlNodePtr node = NULL;
    ListIterator iter = createIterator(temp->groups);
    void *attrElem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        Group *grpTemp = (Group *)elem;
        node = xmlNewChild(outerNode, NULL, BAD_CAST "g", NULL);
        groupRectTree(elem, node);
        groupCircleTree(elem, node);
        groupPathTree(elem, node);
        groupGroupTree(elem, node);

        ListIterator attrIter = createIterator(grpTemp->otherAttributes);
        while ((attrElem = nextElement(&attrIter)) != NULL)
        {
            Attribute *temp2 = (Attribute *)attrElem;
            xmlNewProp(node, BAD_CAST temp2->name, BAD_CAST temp2->value);
        }
        xmlAddChild(outerNode, node);

    }
}

void rectToTree(SVGimage *img, xmlNodePtr root_node)
{
    xmlNodePtr node = NULL;
    ListIterator rectIter = createIterator(img->rectangles);
    void *elem, *attrElem;
    char buffer[255] = "";

    while ((elem = nextElement(&rectIter)) != NULL)
    {
        node = xmlNewChild(root_node, NULL, BAD_CAST "rect", NULL);
        Rectangle *temp = (Rectangle *)elem;
        
        snprintf(buffer, 25, "%f", temp->x);
        strcat(buffer, temp->units);
        xmlNewProp(node, BAD_CAST "x", BAD_CAST buffer);

        snprintf(buffer, 25, "%f", temp->y);
        strcat(buffer, temp->units);
        xmlNewProp(node, BAD_CAST "y", BAD_CAST buffer);

        snprintf(buffer, 25, "%f", temp->width);
        strcat(buffer, temp->units);
        xmlNewProp(node, BAD_CAST "width", BAD_CAST buffer);

        snprintf(buffer, 25, "%f", temp->height);
        strcat(buffer, temp->units);
        xmlNewProp(node, BAD_CAST "height", BAD_CAST buffer);

        ListIterator rectAttrIter = createIterator(temp->otherAttributes);
        while ((attrElem = nextElement(&rectAttrIter)) != NULL)
        {
            Attribute *temp2 = (Attribute *)attrElem;
            xmlNewProp(node, BAD_CAST temp2->name, BAD_CAST temp2->value);
        }
        xmlAddChild(root_node, node);
    }
}

void circleToTree(SVGimage *img, xmlNodePtr root_node)
{
    xmlNodePtr node = NULL;
    ListIterator circleIter = createIterator(img->circles);
    void *elem, *attrElem;
    char buffer[255] = "";

    while ((elem = nextElement(&circleIter)) != NULL)
    {
        node = xmlNewChild(root_node, NULL, BAD_CAST "circle", NULL);
        Circle *temp = (Circle *)elem;

        snprintf(buffer, 25, "%f", temp->cx);
        strcat(buffer, temp->units);
        xmlNewProp(node, BAD_CAST "cx", BAD_CAST buffer);

        snprintf(buffer, 25, "%f", temp->cy);
        strcat(buffer, temp->units);
        xmlNewProp(node, BAD_CAST "cy", BAD_CAST buffer);

        snprintf(buffer, 25, "%f", temp->r);
        strcat(buffer, temp->units);
        xmlNewProp(node, BAD_CAST "r", BAD_CAST buffer);

        ListIterator circleAttrIter = createIterator(temp->otherAttributes);
        while ((attrElem = nextElement(&circleAttrIter)) != NULL)
        {
            Attribute *temp2 = (Attribute *)attrElem;
            xmlNewProp(node, BAD_CAST temp2->name, BAD_CAST temp2->value);
        }
        xmlAddChild(root_node, node);
    }
}

void pathToTree(SVGimage *img, xmlNodePtr root_node)
{
    xmlNodePtr node = NULL;
    ListIterator pathIter = createIterator(img->paths);
    void *elem, *attrElem;

    while ((elem = nextElement(&pathIter)) != NULL)
    {
        node = xmlNewChild(root_node, NULL, BAD_CAST "path", NULL);
        Path *temp = (Path *)elem;
        xmlNewProp(node, BAD_CAST "d", BAD_CAST temp->data);

        ListIterator pathAttrIter = createIterator(temp->otherAttributes);
        while ((attrElem = nextElement(&pathAttrIter)) != NULL)
        {
            Attribute *temp2 = (Attribute *)attrElem;
            xmlNewProp(node, BAD_CAST temp2->name, BAD_CAST temp2->value);
        }
        xmlAddChild(root_node, node);
    }
}

void groupToTree(SVGimage *img, xmlNodePtr root_node)
{
    xmlNodePtr node = NULL;
    ListIterator groupIter = createIterator(img->groups);
    void *elem, *attrElem;

    while ((elem = nextElement(&groupIter)) != NULL)
    {
        Group *temp = (Group *)elem;
        node = xmlNewChild(root_node, NULL, BAD_CAST "g", NULL);
        groupRectTree(elem, node);
        groupCircleTree(elem, node);
        groupPathTree(elem, node);
        groupGroupTree(elem, node);

        ListIterator groupAttrIter = createIterator(temp->otherAttributes);
        while ((attrElem = nextElement(&groupAttrIter)) != NULL)
        {
            Attribute *temp2 = (Attribute *)attrElem;
            xmlNewProp(node, BAD_CAST temp2->name, BAD_CAST temp2->value);
        }
        xmlAddChild(root_node, node);
    }
}
  
xmlDocPtr createTree(SVGimage *img)
{
    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;

    LIBXML_TEST_VERSION;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlDocSetRootElement(doc, root_node);

    xmlNsPtr namespace = xmlNewNs(root_node, (xmlChar *)img->namespace, NULL);
    xmlSetNs(root_node, namespace);

    if (strcmp(img->title, "") != 0)
    {
        xmlNewChild(root_node, NULL, BAD_CAST "title", BAD_CAST img->title);
    }

    if (strcmp(img->description, "") != 0)
    {
        xmlNewChild(root_node, NULL, BAD_CAST "desc", BAD_CAST img->description);
    }

    ListIterator attrIter = createIterator(img->otherAttributes);
    void *elem;

    while ((elem = nextElement(&attrIter)) != NULL)
    {
        Attribute *temp = (Attribute *)elem;
        xmlNewProp(root_node, BAD_CAST temp->name, BAD_CAST temp->value);
    }

    rectToTree(img, root_node);
    circleToTree(img, root_node);
    pathToTree(img, root_node);
    groupToTree(img, root_node);
    return doc;
}

bool validateDoc(xmlDocPtr doc, char* schemaFile)
{
    if (!doc || !schemaFile)
        return false;
    
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(schemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    if (doc == NULL)
        return false;
    else
    {
        xmlSchemaValidCtxtPtr ctxt; 
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, doc);
        if (ret == 0)
        {
            printf("validates\n");
        }
        else if (ret > 0)
        {
            printf("fails to validate\n");
            return false;
        }
        else
        {
            printf("validation generated an internal error\n");
            return false;
        }
        xmlSchemaFreeValidCtxt(ctxt);
    }

    if (schema != NULL)
        xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
    return true;
}

bool validateAttributes(List *list)
{
    ListIterator iter = createIterator(list);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        Attribute *temp = (Attribute *)elem;
        if (!temp->name)
        {
            return false;
        }
    }
    return true;
}

bool validateRects(SVGimage *img)
{
    List *rectList = getRects(img);
    ListIterator iter = createIterator(rectList);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        Rectangle *temp = (Rectangle *)elem;

        if (!temp->otherAttributes)
            return false;

        if (temp->width < 0 || temp->height < 0)
        {
            freeList(rectList);
            return false;
        }

        if (validateAttributes(temp->otherAttributes) == false)
        {
            freeList(rectList);
            return false;
        }
    }
    freeList(rectList);
    return true;
}

bool validateCircs(SVGimage *img)
{
    List *circList = getCircles(img);
    ListIterator iter = createIterator(circList);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        Circle *temp = (Circle *)elem;

        if (!temp->otherAttributes)
            return false;

        if (temp->r < 0)
        {
            freeList(circList);
            return false;
        }

        if (validateAttributes(temp->otherAttributes) == false)
        {
            freeList(circList);
            return false;
        }
    }
    freeList(circList);
    return true;
}

bool validatePaths(SVGimage *img)
{
    List *pathList = getPaths(img);
    ListIterator iter = createIterator(pathList);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        Path *temp = (Path *)elem;

        if (!temp->otherAttributes)
            return false;

        if (strlen(temp->data) <= 0)
        {
            freeList(pathList);
            return false;
        }

        if (validateAttributes(temp->otherAttributes) == false)
        {
            freeList(pathList);
            return false;
        }
    }
    freeList(pathList);
    return true;
}

bool validateConstraints(SVGimage *img)
{
    if (strlen(img->namespace) <= 0)
        return false;

    if (validateRects(img) == false)
        return false;

    if (validateCircs(img) == false)
        return false;

    if (validatePaths(img) == false)
        return false;

    if (validateAttributes(img->otherAttributes) == false)
        return false;

    return true;
}

bool validateSVGimage(SVGimage* doc, char* schemaFile)
{
    if (!doc || !doc->rectangles || !doc->circles || !doc->paths || !doc->groups || !doc->otherAttributes || !schemaFile)
        return false;

    if (validateConstraints(doc) == false)
        return false;

    xmlDocPtr ptr = createTree(doc);

    if (validateDoc(ptr, schemaFile) == false)
        return false;

    xmlFreeDoc(ptr);
    return true;
}

SVGimage* createValidSVGimage(char* fileName, char* schemaFile)
{
    if (!fileName || !schemaFile)
        return NULL;

    SVGimage *img = createSVGimage(fileName);

    if (validateSVGimage(img, schemaFile) == false)
        return NULL;

    return img;
}

bool writeSVGimage(SVGimage* doc, char* fileName)
{
    if (!doc || !fileName)
        return false;

    xmlDocPtr docPtr = createTree(doc);

    xmlSaveFormatFileEnc(fileName, docPtr, "UTF-8", 1);
    xmlFreeDoc(docPtr);
    xmlCleanupParser();
    xmlMemoryDump();
    return true;
}

void setRectAttr(SVGimage* image, int elemIndex, Attribute* newAttribute)
{
    int counter = 0;
    int success = 0;
    ListIterator iter = createIterator(image->rectangles);
    void *elem, *attrElem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        if (counter == elemIndex)
        {
            Rectangle *temp = (Rectangle *)elem;
            if (strcmp(newAttribute->name, "x") == 0)
            {
                temp->x = atof(newAttribute->value);
                deleteAttribute(newAttribute);
            }
            else if (strcmp(newAttribute->name, "y") == 0)
            {
                temp->y = atof(newAttribute->value);
                deleteAttribute(newAttribute);
            }
            else if (strcmp(newAttribute->name, "width") == 0)
            {
                temp->width = atof(newAttribute->value);
                deleteAttribute(newAttribute);
            }
            else if (strcmp(newAttribute->name, "height") == 0)
            {
                temp->height = atof(newAttribute->value);
                deleteAttribute(newAttribute);
            }
            else if (strcmp(newAttribute->name, "units") == 0)
            {
                strcpy(temp->units, newAttribute->value);
                deleteAttribute(newAttribute);
            }
            else
            {
                ListIterator attrIter = createIterator(temp->otherAttributes);

                while ((attrElem = nextElement(&attrIter)) != NULL)
                {
                    Attribute *temp2 = (Attribute *)attrElem;
                    if (strcmp(temp2->name, newAttribute->name) == 0)
                    {
                        strcpy(temp2->value, newAttribute->value);
                        deleteAttribute(newAttribute);
                        success++;
                        break;
                    }
                }
                
                if (success == 0)
                {
                    insertBack(temp->otherAttributes, newAttribute);
                }
            }
            break;
        }
        counter++;
    }
}

void setCircAttr(SVGimage* image, int elemIndex, Attribute* newAttribute)
{
    int counter = 0;
    int success = 0;
    ListIterator iter = createIterator(image->circles);
    void *elem, *attrElem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        if (counter == elemIndex)
        {
            Circle *temp = (Circle *)elem;
            if (strcmp(newAttribute->name, "cx") == 0)
            {
                temp->cx = atof(newAttribute->value);
                deleteAttribute(newAttribute);
            }
            else if (strcmp(newAttribute->name, "cy") == 0)
            {
                temp->cy = atof(newAttribute->value);
                deleteAttribute(newAttribute);
            }
            else if (strcmp(newAttribute->name, "r") == 0)
            {
                temp->r = atof(newAttribute->value);
                deleteAttribute(newAttribute);
            }
            else if (strcmp(newAttribute->name, "units") == 0)
            {
                strcpy(temp->units, newAttribute->value);
                deleteAttribute(newAttribute);
            }
            else
            {
                ListIterator attrIter = createIterator(temp->otherAttributes);
                
                while ((attrElem = nextElement(&attrIter)) != NULL)
                {
                    Attribute *temp2 = (Attribute *)attrElem;
                    if (strcmp(temp2->name, newAttribute->name) == 0)
                    {
                        strcpy(temp2->value, newAttribute->value);
                        deleteAttribute(newAttribute);
                        success++;
                        break;
                    }
                }

                if (success == 0)
                {
                    insertBack(temp->otherAttributes, newAttribute);
                }
            }
            break;
        }
        counter++;
    }
}

void setPathAttr(SVGimage* image, int elemIndex, Attribute* newAttribute)
{
    int counter = 0;
    int success = 0;
    ListIterator iter = createIterator(image->paths);
    void *elem, *attrElem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        if (counter == elemIndex)
        {
            Path *temp = (Path *)elem;
            if (strcmp(newAttribute->name, "d") == 0)
            {
                strcpy(temp->data, newAttribute->value);
                deleteAttribute(newAttribute);
            }
            else
            {
                ListIterator attrIter = createIterator(temp->otherAttributes);
                
                while ((attrElem = nextElement(&attrIter)) != NULL)
                {
                    Attribute *temp2 = (Attribute *)attrElem;
                    if (strcmp(temp2->name, newAttribute->name) == 0)
                    {
                        strcpy(temp2->value, newAttribute->value);
                        deleteAttribute(newAttribute);
                        success++;
                        break;
                    }
                }

                if (success == 0)
                {
                    insertBack(temp->otherAttributes, newAttribute);
                }
            }
            break;
        }
        counter++;
    }
}

void setGroupAttr(SVGimage* image, int elemIndex, Attribute* newAttribute)
{
    int counter = 0;
    int success = 0;
    ListIterator iter = createIterator(image->groups);
    void *elem, *attrElem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        if (counter == elemIndex)
        {
            Group *temp = (Group *)elem;
            ListIterator attrIter = createIterator(temp->otherAttributes);
            
            while ((attrElem = nextElement(&attrIter)) != NULL)
            {
                Attribute *temp2 = (Attribute *)attrElem;
                if (strcmp(temp2->name, newAttribute->name) == 0)
                {
                    strcpy(temp2->value, newAttribute->value);
                    deleteAttribute(newAttribute);
                    success++;
                    break;
                }
            }

            if (success == 0)
            {
                insertBack(temp->otherAttributes, newAttribute);
            }
            break;
        }
        counter++;
    }
}

void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute)
{
    if (!image || elemType < 0 || elemType > 4 || !newAttribute)
        return;
    
    int success = 0;
    ListIterator iter = createIterator(image->otherAttributes);
    void *elem;

    if (elemType == SVG_IMAGE)
    {
        while ((elem = nextElement(&iter)) != NULL)
        {
            Attribute *temp = (Attribute *)elem;

            if (strcmp(temp->name, newAttribute->name) == 0)
            {
                strcpy(temp->value, newAttribute->value);
                deleteAttribute(newAttribute);
                success++;
                break;
            }
        }

        if (success == 0)
        {
            insertBack(image->otherAttributes, newAttribute);
        }
    }

    if (elemIndex < 0)
        return;

    if (elemType == RECT)
    {
        setRectAttr(image, elemIndex, newAttribute);
    }
    else if (elemType == CIRC)
    {
        setCircAttr(image, elemIndex, newAttribute);
    }
    else if (elemType == PATH)
    {
        setPathAttr(image, elemIndex, newAttribute);
    }
    else if (elemType == GROUP)
    {
        setGroupAttr(image, elemIndex, newAttribute);
    }
}

void addComponent(SVGimage* image, elementType type, void* newElement)
{
    if (!image || type < 0 || type > 4 || !newElement)
        return;

    if (type == RECT)
    {
        Rectangle *temp = (Rectangle *)newElement;
        insertBack(image->rectangles, temp);
    }
    else if (type == CIRC)
    {
        Circle *temp = (Circle *)newElement;
        insertBack(image->circles, temp);
    }
    else if (type == PATH)
    {
        Path *temp = (Path *)newElement;
        insertBack(image->paths, temp);
    }
}

char* attrToJSON(const Attribute *a)
{
    if (!a)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "{}\0");
        return empString;
    }

    char *string = malloc(sizeof(char) * (strlen(a->name) + strlen(a->value)) + 23);

    strcpy(string, "{\"name\":");
    strcat(string, "\"");
    strcat(string, a->name);
    strcat(string, "\"");
    strcat(string, ",");
    strcat(string, "\"value\":");
    strcat(string, "\"");
    strcat(string, a->value);
    strcat(string, "\"}");
    return string;
}

char* circleToJSON(const Circle *c)
{
    if (!c)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "{}\0");
        return empString;
    }

    char buffer[50] = "";
    char buffer2[50] = "";
    char buffer3[50] = "";
    char buffer4[50] = "";

    snprintf(buffer, 25, "%0.2f", c->cx);
    snprintf(buffer2, 25, "%0.2f", c->cy);
    snprintf(buffer3, 25, "%0.2f", c->r);
    snprintf(buffer4, 25, "%d", c->otherAttributes->length);

    char *string = malloc(sizeof(char) *(strlen(c->units) + strlen(buffer) + strlen(buffer2) + strlen(buffer3)) + 42);

    strcpy(string, "{\"cx\":");
    strcat(string, buffer);
    strcat(string, ",");
    strcat(string, "\"cy\":");
    strcat(string, buffer2);
    strcat(string, ",");
    strcat(string, "\"r\":");
    strcat(string, buffer3);
    strcat(string, ",");
    strcat(string, "\"numAttr\":");
    strcat(string, buffer4);
    strcat(string, ",");
    strcat(string, "\"units\":");
    strcat(string, "\"");
    strcat(string, c->units);
    strcat(string, "\"");
    strcat(string, "}");
    return string;
}

char* rectToJSON(const Rectangle *r)
{
    if (!r)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "{}\0");
        return empString;
    }

    char buffer[50] = "";
    char buffer2[50] = "";
    char buffer3[50] = "";
    char buffer4[50] = "";
    char buffer5[50] = "";

    snprintf(buffer, 25, "%0.2f", r->x);
    snprintf(buffer2, 25, "%0.2f", r->y);
    snprintf(buffer3, 25, "%0.2f", r->width);
    snprintf(buffer4, 25, "%0.2f", r->height);
    snprintf(buffer5, 25, "%d", r->otherAttributes->length);

    char *string = malloc(sizeof(char) *(strlen(r->units) + strlen(buffer) + strlen(buffer2) + strlen(buffer3) + strlen(buffer4)) + 45);

    strcpy(string, "{\"x\":");
    strcat(string, buffer);
    strcat(string, ",");
    strcat(string, "\"y\":");
    strcat(string, buffer2);
    strcat(string, ",");
    strcat(string, "\"w\":");
    strcat(string, buffer3);
    strcat(string, ",");
    strcat(string, "\"h\":");
    strcat(string, buffer4);
    strcat(string, ",");
    strcat(string, "\"numAttr\":");
    strcat(string, buffer5);
    strcat(string, ",");
    strcat(string, "\"units\":");
    strcat(string, "\"");
    strcat(string, r->units);
    strcat(string, "\"");
    strcat(string, "}");
    return string;
}

char* pathToJSON(const Path *p)
{
    if (!p)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "{}\0");
        return empString;
    }

    char buffer[50] = "";
    char buffer2[100] = "";

    snprintf(buffer, 25, "%d", p->otherAttributes->length);
    strncpy(buffer2, p->data, 64);
    strcat(buffer2, "\0");

    char *string = malloc(sizeof(char) * (strlen(buffer2)) + 30);

    strcpy(string, "{\"d\":");
    strcat(string, "\"");
    strcat(string, buffer2);
    strcat(string, "\"");
    strcat(string, ",");
    strcat(string, "\"numAttr\":");
    strcat(string, buffer);
    strcat(string, "}");
    return string;
}

char* groupToJSON(const Group *g)
{
    if (!g)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "{}\0");
        return empString;
    }

    int totalChilds = g->rectangles->length + g->circles->length + g->paths->length + g->groups->length;
    char buffer[50] = "";
    char buffer2[50] = "";

    snprintf(buffer, 25, "%d", totalChilds);
    snprintf(buffer2, 25, "%d", g->otherAttributes->length);

    char *string = malloc(sizeof(char) * (strlen(buffer) + strlen(buffer2)) + 30);

    strcpy(string, "{\"children\":");
    strcat(string, buffer);
    strcat(string, ",");
    strcat(string, "\"numAttr\":");
    strcat(string, buffer2);
    strcat(string, "}");
    return string;
}

char* attrListToJSON(const List *list)
{
    if (!list)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "[]\0");
        return empString;
    }

    ListIterator iter = createIterator((List *)list);
    void *elem;
    char *string = malloc(sizeof(char) * 2);
    char *buffer;

    strcpy(string, "[");
    while ((elem = nextElement(&iter)) != NULL)
    {
        Attribute *temp = (Attribute *)elem;
        buffer = attrToJSON(temp);
        string = realloc(string, strlen(string) + strlen(buffer) + 3);
        strcat(string, buffer);
        strcat(string, ",");
        free(buffer);
    }
    
    if (strlen(string) < 5)
    {
        free(string);
        char *noList = malloc(sizeof(char) * 3);
        strcpy(noList, "[]\0");
        return noList;
    }

    string[strlen(string) - 1] = ']';
    return string;
}

char* circListToJSON(const List *list)
{
    if (!list)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "[]\0");
        return empString;
    }

    ListIterator iter = createIterator((List *)list);
    void *elem;
    char *string = malloc(sizeof(char) * 2);
    char *buffer;

    strcpy(string, "[");
    while ((elem = nextElement(&iter)) != NULL)
    {
        Circle *temp = (Circle *)elem;
        buffer = circleToJSON(temp);
        string = realloc(string, strlen(string) + strlen(buffer) + 3);
        strcat(string, buffer);
        strcat(string, ",");
        free(buffer);
    }
    
    if (strlen(string) < 5)
    {
        free(string);
        char *noList = malloc(sizeof(char) * 3);
        strcpy(noList, "[]\0");
        return noList;
    }

    string[strlen(string) - 1] = ']';
    return string;
}

char* rectListToJSON(const List *list)
{
    if (!list)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "[]\0");
        return empString;
    }

    ListIterator iter = createIterator((List *)list);
    void *elem;
    char *string = malloc(sizeof(char) * 2);
    char *buffer;

    strcpy(string, "[");
    while ((elem = nextElement(&iter)) != NULL)
    {
        Rectangle *temp = (Rectangle *)elem;
        buffer = rectToJSON(temp);
        string = realloc(string, strlen(string) + strlen(buffer) + 3);
        strcat(string, buffer);
        strcat(string, ",");
        free(buffer);
    }
    
    if (strlen(string) < 5)
    {
        free(string);
        char *noList = malloc(sizeof(char) * 3);
        strcpy(noList, "[]\0");
        return noList;
    }

    string[strlen(string) - 1] = ']';
    return string;
}

char* pathListToJSON(const List *list)
{
    if (!list)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "[]\0");
        return empString;
    }

    ListIterator iter = createIterator((List *)list);
    void *elem;
    char *string = malloc(sizeof(char) * 2);
    char *buffer;

    strcpy(string, "[");
    while ((elem = nextElement(&iter)) != NULL)
    {
        Path *temp = (Path *)elem;
        buffer = pathToJSON(temp);
        string = realloc(string, strlen(string) + strlen(buffer) + 3);
        strcat(string, buffer);
        strcat(string, ",");
        free(buffer);
    }
    
    if (strlen(string) < 5)
    {
        free(string);
        char *noList = malloc(sizeof(char) * 3);
        strcpy(noList, "[]\0");
        return noList;
    }

    string[strlen(string) - 1] = ']';
    return string;
}

char* groupListToJSON(const List *list)
{
    if (!list)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "[]\0");
        return empString;
    }

    ListIterator iter = createIterator((List *)list);
    void *elem;
    char *string = malloc(sizeof(char) * 2);
    char *buffer;

    strcpy(string, "[");
    while ((elem = nextElement(&iter)) != NULL)
    {
        Group *temp = (Group *)elem;
        buffer = groupToJSON(temp);
        string = realloc(string, strlen(string) + strlen(buffer) + 3);
        strcat(string, buffer);
        strcat(string, ",");
        free(buffer);
    }
    
    if (strlen(string) < 5)
    {
        free(string);
        char *noList = malloc(sizeof(char) * 3);
        strcpy(noList, "[]\0");
        return noList;
    }

    string[strlen(string) - 1] = ']';
    return string;
}

char* SVGtoJSON(const SVGimage* imge)
{
    if (!imge)
    {
        char *empString = malloc(sizeof(char) * 3);
        strcpy(empString, "{}");
        return empString;
    }

    List *rectList = getRects((SVGimage *)imge);
    List *circList = getCircles((SVGimage *)imge);
    List *pathList = getPaths((SVGimage *)imge);
    List *groupList = getGroups((SVGimage *)imge);

    char buffer[50] = "";
    char buffer2[50] = "";
    char buffer3[50] = "";
    char buffer4[50] = "";

    snprintf(buffer, 25, "%d", rectList->length);
    snprintf(buffer2, 25, "%d", circList->length);
    snprintf(buffer3, 25, "%d", pathList->length);
    snprintf(buffer4, 25, "%d", groupList->length);

    char *string = malloc(sizeof(char) * (strlen(buffer) + strlen(buffer2) + strlen(buffer3) + strlen(buffer4)) + 55);

    strcpy(string, "{\"numRect\":");
    strcat(string, buffer);
    strcat(string, ",");
    strcat(string, "\"numCirc\":");
    strcat(string, buffer2);
    strcat(string, ",");
    strcat(string, "\"numPaths\":");
    strcat(string, buffer3);
    strcat(string, ",");
    strcat(string, "\"numGroups\":");
    strcat(string, buffer4);
    strcat(string, "}");

    freeList(rectList);
    freeList(circList);
    freeList(pathList);
    freeList(groupList);
    return string;
}

SVGimage* JSONtoSVG(const char* svgString)
{
    if (!svgString)
        return NULL;

    SVGimage *img = malloc(sizeof(SVGimage));
    initLists(img);
    int i = 0;
    char buffer[100] = "";
    char buffer2[100] = "";

    strcpy(img->namespace, "http://www.w3.org/2000/svg");
    strcpy(img->title, "");
    strcpy(img->description, "");

    svgString = svgString + 10;
    while (svgString[i] != '\"')
    {
        buffer[i] = svgString[i];
        i++;
    }
    strcat(buffer, "\0");
    strcpy(img->title, buffer);

    svgString = svgString + strlen(img->title) + 11;
    i = 0;
    while (svgString[i] != '\"')
    {
        buffer2[i] = svgString[i];
        i++;
    }
    strcat(buffer2, "\0");
    strcpy(img->description, buffer2);

    return img;
}

Rectangle* JSONtoRect(const char* svgString)
{
    if (!svgString)
        return NULL;

    Rectangle *rect = malloc(sizeof(Rectangle));
    int i = 0;
    char buffer[50] = "";
    char buffer2[50] = "";
    char buffer3[50] = "";
    char buffer4[50] = "";
    char buffer5[50] = "";

    rect->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    rect->x = 0;
    rect->y = 0;
    rect->width = 0;
    rect->height = 0;
    strcpy(rect->units, "");

    svgString = svgString + 5;
    while (svgString[i] != ',')
    {
        buffer[i] = svgString[i];
        i++;
    }
    strcat(buffer, "\0");
    rect->x = atof(buffer);

    svgString = svgString + strlen(buffer) + 5;
    i = 0;
    while (svgString[i] != ',')
    {
        buffer2[i] = svgString[i];
        i++;
    }    
    strcat(buffer2, "\0");
    rect->y = atof(buffer2);

    svgString = svgString + strlen(buffer2) + 5;
    i = 0;
    while (svgString[i] != ',')
    {
        buffer3[i] = svgString[i];
        i++;
    }
    strcat(buffer3, "\0");
    rect->width = atof(buffer3);

    svgString = svgString + strlen(buffer3) + 5;
    i = 0;
    while (svgString[i] != ',')
    {
        buffer4[i] = svgString[i];
        i++;
    }
    strcat(buffer4, "\0");
    rect->height = atof(buffer4);

    svgString = svgString + strlen(buffer4) + 10;
    i = 0;
    while (svgString[i] != '\"')
    {
        buffer5[i] = svgString[i];
        i++;
    }
    strcat(buffer5, "\0");
    strcpy(rect->units, buffer5);
    return rect;
}

Circle* JSONtoCircle(const char* svgString)
{
    if (!svgString)
        return NULL;

    Circle *circ = malloc(sizeof(Circle));
    int i = 0;
    char buffer[50] = "";
    char buffer2[50] = "";
    char buffer3[50] = "";
    char buffer4[50] = "";

    circ->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    circ->cx = 0;
    circ->cy = 0;
    circ->r = 0;
    strcpy(circ->units, "");

    svgString = svgString + 6;
    while (svgString[i] != ',')
    {
        buffer[i] = svgString[i];
        i++;
    }
    strcat(buffer, "\0");
    circ->cx = atof(buffer);

    svgString = svgString + strlen(buffer) + 6;
    i = 0;
    while (svgString[i] != ',')
    {
        buffer2[i] = svgString[i];
        i++;
    }    
    strcat(buffer2, "\0");
    circ->cy = atof(buffer2);

    svgString = svgString + strlen(buffer2) + 5;
    i = 0;
    while (svgString[i] != ',')
    {
        buffer3[i] = svgString[i];
        i++;
    }
    strcat(buffer3, "\0");
    circ->r = atof(buffer3);

    svgString = svgString + strlen(buffer3) + 10;
    i = 0;
    while (svgString[i] != '\"')
    {
        buffer4[i] = svgString[i];
        i++;
    }
    strcat(buffer4, "\0");
    strcpy(circ->units, buffer4);
    return circ;
}

/* JAVASCRIPT WRAPPER FUNCTIONS */
char *parseSVGtoJSON(char *fileName)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    char *string = SVGtoJSON(img);
    deleteSVGimage(img);
    return string;
}

char *titleDescJSON(char *fileName)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    char buffer[100];
    char buffer2[250];
    
    strcpy(buffer, img->title);
    strcpy(buffer2, img->description);
    
    char *string = malloc(sizeof(char) * (strlen(buffer) + strlen(buffer2) + 250));

    strcpy(string, "{\"title\":");
    strcat(string, "\"");
    strcat(string, buffer);
    strcat(string, "\"");
    strcat(string, ",");
    strcat(string, "\"description\":");
    strcat(string, "\"");
    strcat(string, buffer2);
    strcat(string, "\"");
    strcat(string, "}");
    deleteSVGimage(img);
    return string;
}

char *getRectJSON(char *fileName)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    char *string = rectListToJSON(img->rectangles);
    deleteSVGimage(img);
    return string;
}

char *getCircJSON(char *fileName)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    char *string = circListToJSON(img->circles);
    deleteSVGimage(img);
    return string;
}

char *getPathJSON(char *fileName)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    char *string = pathListToJSON(img->paths);
    deleteSVGimage(img);
    return string;
}

char *getGroupJSON(char *fileName)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    char *string = groupListToJSON(img->groups);
    deleteSVGimage(img);
    return string;
}

bool addCircle(char *fileName, char *JSONstring)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    Circle *circle = JSONtoCircle(JSONstring);

    addComponent(img, CIRC, circle);
    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            deleteSVGimage(img);
            return true;
        }
    }
    deleteSVGimage(img);
    return false;
}

bool addRectangle(char *fileName, char *JSONstring)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    Rectangle *rect = JSONtoRect(JSONstring);

    addComponent(img, RECT, rect);
    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            deleteSVGimage(img);
            return true;
        }
    }
    deleteSVGimage(img);
    return false;
}

bool editTitle(char *fileName, char *string)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    strncpy(img->title, string, 254);
    strcat(img->title, "\0");

    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            deleteSVGimage(img);
            return true;
        }
    }
    deleteSVGimage(img);
    return false;
}

bool editDesc(char *fileName, char *string)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    strncpy(img->description, string, 254);
    strcat(img->description, "\0");

    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            deleteSVGimage(img);
            return true;
        }
    }
    deleteSVGimage(img);
    return false;
}

char *showAttrCirc(char *fileName, int i)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    List *circList = getCircles(img);
    int counter = 0;
    int index = i-1;
    char *string = NULL;

    ListIterator iter = createIterator(circList);
    void *elem;

    while ((elem = nextElement(&iter)) != 0)
    {
        if (counter == (index))
        {
            Circle *temp = (Circle *)elem;
            string = attrListToJSON(temp->otherAttributes);
        }
        counter++;
    }
    deleteSVGimage(img);
    freeList(circList);
    return string;
}

char *showAttrRect(char *fileName, int i)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    List *List = getRects(img);
    int counter = 0;
    int index = i-1;
    char *string = NULL;

    ListIterator iter = createIterator(List);
    void *elem;

    while ((elem = nextElement(&iter)) != 0)
    {
        if (counter == (index))
        {
            Rectangle *temp = (Rectangle *)elem;
            string = attrListToJSON(temp->otherAttributes);
        }
        counter++;
    }
    deleteSVGimage(img);
    freeList(List);
    return string;
}

char *showAttrPath(char *fileName, int i)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    List *List = getPaths(img);
    int counter = 0;
    int index = i-1;
    char *string = NULL;

    ListIterator iter = createIterator(List);
    void *elem;

    while ((elem = nextElement(&iter)) != 0)
    {
        if (counter == (index))
        {
            Path *temp = (Path *)elem;
            string = attrListToJSON(temp->otherAttributes);
        }
        counter++;
    }
    deleteSVGimage(img);
    freeList(List);
    return string;
}

char *showAttrGroup(char *fileName, int i)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    List *List = getGroups(img);
    int counter = 0;
    int index = i-1;
    char *string = NULL;

    ListIterator iter = createIterator(List);
    void *elem;

    while ((elem = nextElement(&iter)) != 0)
    {
        if (counter == (index))
        {
            Group *temp = (Group *)elem;
            string = attrListToJSON(temp->otherAttributes);
        }
        counter++;
    }
    deleteSVGimage(img);
    freeList(List);
    return string;
}

bool scaleRects(char *fileName, float scaler)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    List *list = getRects(img);
    
    ListIterator iter = createIterator(list);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        Rectangle *temp = (Rectangle *)elem;
        temp->x = temp->x * scaler;
        temp->y = temp->y * scaler;
    }

    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            freeList(list);
            deleteSVGimage(img);
            return true;
        }
    }

    freeList(list);
    deleteSVGimage(img);
    return false;
}

bool scaleCircles(char *fileName, float scaler)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    List *list = getCircles(img);
    
    ListIterator iter = createIterator(list);
    void *elem;

    while ((elem = nextElement(&iter)) != NULL)
    {
        Circle *temp = (Circle *)elem;
        temp->cx = temp->cx * scaler;
        temp->cy = temp->cy * scaler;
    }

    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            freeList(list);
            deleteSVGimage(img);
            return true;
        }
    }

    freeList(list);
    deleteSVGimage(img);
    return false;
}

bool newSVGimage(char *fileName)
{
    SVGimage *img = malloc(sizeof(SVGimage));
    initLists(img);
    strcpy(img->namespace, "http://www.w3.org/2000/svg");
    strcpy(img->title, "This is a Title");
    strcpy(img->description, "This is a Description");
    char *string = malloc(sizeof(char) * 25);
    strcpy(string, "./uploads/");
    strcat(string, fileName);
    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, string))
        {
            deleteSVGimage(img);
            free(string);
            return true;
        }
    }
    free(string);
    deleteSVGimage(img);
    return false;
}

bool editAttribute(char *fileName, char *shape, int index, char *attrName, char *attrValue)
{
    printf("SHAPE IS %s, INDEX IS %d, ATTRNAME IS %s, ATTRVALUE IS %s\n", shape, index, attrName, attrValue);
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    Attribute *newAttr = malloc(sizeof(Attribute));
    newAttr->name = malloc(sizeof(char) * strlen(attrName) + 1);
    newAttr->value = malloc(sizeof(char) * strlen(attrValue) + 1);
    strcpy(newAttr->name, attrName);
    strcpy(newAttr->value, attrValue);

    if (strcmp(shape, "rectangle") == 0)
        setAttribute(img, RECT, index, newAttr);

    if (strcmp(shape, "circle") == 0)
        setAttribute(img, CIRC, index, newAttr);
    
    if (strcmp(shape, "path") == 0)
        setAttribute(img, PATH, index, newAttr);

    if (strcmp(shape, "group") == 0)
        setAttribute(img, GROUP, index, newAttr);

    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            deleteSVGimage(img);
            return true;
        }
    }
    deleteSVGimage(img);
    return false;
}

bool editRect(char *fileName, int index, char *attrName, char *attrValue)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    float value = atof(attrValue);

    List *list = getRects(img);
    ListIterator iter = createIterator(list);
    void *elem;
    int counter = 0;

    while ((elem = nextElement(&iter)) != NULL)
    {
        if (counter == index)
        {
            Rectangle *temp = (Rectangle *)elem;
            if (strcmp(attrName, "x") == 0)
                temp->x = value;
            else if (strcmp(attrName, "y") == 0)
                temp->y = value;
            else if (strcmp(attrName, "w") == 0)
                temp->width = value;
            else if (strcmp(attrName, "h") == 0)
                temp->height = value;
        }
        counter++;
    }

    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            freeList(list);
            deleteSVGimage(img);
            return true;
        }
    }
    freeList(list);
    deleteSVGimage(img);
    return false;
}

bool editRectUnits(char *fileName, int index, char *attrName, char *attrValue)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    List *list = getRects(img);
    ListIterator iter = createIterator(list);
    void *elem;
    int counter = 0;

    while ((elem = nextElement(&iter)) != NULL)
    {
        if (counter == index)
        {
            Rectangle *temp = (Rectangle *)elem;
            strcpy(temp->units, attrValue);
        }
        counter++;
    }

    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            freeList(list);
            deleteSVGimage(img);
            return true;
        }
    }
    freeList(list);
    deleteSVGimage(img);
    return false;
}

bool editCirc(char *fileName, int index, char *attrName, char *attrValue)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    float value = atof(attrValue);

    List *list = getCircles(img);
    ListIterator iter = createIterator(list);
    void *elem;
    int counter = 0;

    while ((elem = nextElement(&iter)) != NULL)
    {
        if (counter == index)
        {
            Circle *temp = (Circle *)elem;
            if (strcmp(attrName, "cx") == 0)
                temp->cx = value;
            else if (strcmp(attrName, "cy") == 0)
                temp->cy = value;
            else if (strcmp(attrName, "r") == 0)
                temp->r = value;
        }
        counter++;
    }

    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            freeList(list);
            deleteSVGimage(img);
            return true;
        }
    }
    freeList(list);
    deleteSVGimage(img);
    return false;
}

bool editCircUnits(char *fileName, int index, char *attrName, char *attrValue)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    List *list = getCircles(img);
    ListIterator iter = createIterator(list);
    void *elem;
    int counter = 0;

    while ((elem = nextElement(&iter)) != NULL)
    {
        if (counter == index)
        {
            Circle *temp = (Circle *)elem;
            strcpy(temp->units, attrValue);
        }
        counter++;
    }

    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            freeList(list);
            deleteSVGimage(img);
            return true;
        }
    }
    freeList(list);
    deleteSVGimage(img);
    return false;
}

bool editPath(char *fileName, int index, char *attrValue)
{
    SVGimage *img = createValidSVGimage(fileName, "parser/svg.xsd");
    List *list = getPaths(img);
    ListIterator iter = createIterator(list);
    void *elem;
    int counter = 0;

    while ((elem = nextElement(&iter)) != NULL)
    {
        if (counter == index)
        {
            Path *temp = (Path *)elem;
            strcpy(temp->data, attrValue);
        }
        counter++;
    }

    if (validateSVGimage(img, "parser/svg.xsd"))
    {
        if (writeSVGimage(img, fileName))
        {
            freeList(list);
            deleteSVGimage(img);
            return true;
        }
    }
    freeList(list);
    deleteSVGimage(img);
    return false;
}
