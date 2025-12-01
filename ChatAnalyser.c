#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define Xusers 50
#define Xname  100
#define Xline 1024

struct User 
{
    char name[Xname];
    int messages;
    int totalLength;
};

struct User users[Xusers];
int userCount = 0;
int messagesPerHour[24] = {0};
int Adduser(char *name) 
{
    
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].name, name) == 0)
            return i;
    }
    strcpy(users[userCount].name, name);
    users[userCount].messages = 0;
    users[userCount].totalLength = 0;
    return userCount++;
}

void Sort() 
{
    for (int i = 0; i < userCount - 1; i++) 
    {
        for (int j = i + 1; j < userCount; j++) 
        {
            if (users[i].messages > users[j].messages) 
            {
                struct User temp = users[i];
                users[i] = users[j];
                users[j] = temp;
            }
        }
    }
}

int main() 
{
    FILE *fp = fopen("chat.txt", "r");
    if (!fp) 
    {
        printf("Error\n");
        return 1;
    }

    char line[Xline];
    char currentUser[Xname] = "";

    while (fgets(line, sizeof(line), fp)) 
    {
        line[strcspn(line, "\n")] = 0;  

        if (strchr(line, '-') && strchr(line, ':')) 
        {
            char date[30], time[30], rest[Xline];
            if (sscanf(line, "%[^,], %[^-] - %[^\n]", date, time, rest) == 3) 
            {
                char *colon = strchr(rest, ':');
                if (colon) 
                {
                    *colon = '\0';
                    char user[Xname];
                    strcpy(user, rest);
                    strcpy(currentUser, user);
                    char *msg = colon + 1;
                    int idx = Adduser(user);
                    users[idx].messages++;
                    users[idx].totalLength += strlen(msg);
                    int hour = 0;
                    sscanf(time, "%d", &hour);
                    if (strstr(time, "PM") && hour != 12) hour += 12;
                    if (strstr(time, "AM") && hour == 12) hour = 0;
                    if (hour >= 0 && hour < 24)
                        messagesPerHour[hour]++;
                }
            }
        } 
        else 
        {
            if (strlen(currentUser) > 0) 
            {
                int idx = Adduser(currentUser);
                users[idx].totalLength += strlen(line);
            }
        }
    }

    fclose(fp);

    printf("Chat Statistics:\n");
    Sort();

    for (int i = 0; i < userCount; i++) 
    {
        double avg = 0;
        if (users[i].messages > 0)
            avg = (double)users[i].totalLength / users[i].messages;
        printf("%-20s | Messages: %3d | Avg length: %.1f\n", users[i].name, users[i].messages, avg);
    }

    int maxHour = 0;
    for (int i = 1; i < 24; i++) 
    {
        if (messagesPerHour[i] > messagesPerHour[maxHour])
            maxHour = i;
    }

    printf("Most Active Hour: %02d:00 - %02d:00\n",
           maxHour, (maxHour + 1) % 24);

    return 0;
}
