/****************************************************
 * Title: Computer Networks HW2 : Routing Protocols
 * Summary: Program managing routing table by
            Link State routing algorithm.
 *  |Date                   |Author
    |2023-06-03             |Junyeong JANG
****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODE 100
#define MAX_MESSAGE 1000
#define MAX_LINE 100
#define INF 999999999

typedef struct _Message
{
    char text[MAX_MESSAGE];
    int from, to;
} Message;
Message message[MAX_LINE];

typedef struct _Table
{
    int next, cost;
} Table;
Table routing_table[MAX_NODE][MAX_NODE];

int graph[MAX_NODE][MAX_NODE] = {0};
int max_node;

/* function headers */
void init_graph();
void init_routing_table();
void routing_dijkstra(int);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("usage: linkstate topologyfile messagesfile changesfile\n");
        return 1;
    }

    FILE *topology_fd, *messages_fd, *changes_fd, *output_fd;
    topology_fd = fopen(argv[1], "r");
    messages_fd = fopen(argv[2], "r");
    changes_fd = fopen(argv[3], "r");
    output_fd = fopen("output_ls.txt", "w+");
    if (topology_fd == NULL || messages_fd == NULL || changes_fd == NULL)
    {
        printf("Error: open input file.\n");
        return 1;
    }

    init_graph();
    int node_1, node_2, cost;
    char text[MAX_MESSAGE];
    fscanf(topology_fd, "%d", &max_node);
    while (fscanf(topology_fd, "%d %d %d", &node_1, &node_2, &cost) != EOF)
    {
        graph[node_1][node_2] = cost;
        graph[node_2][node_1] = cost;
    }
    int message_top = 0;
    while (fscanf(messages_fd, "%d %d %[^\n]s", &node_1, &node_2, text) != EOF)
    {
        message[message_top].from = node_1;
        message[message_top].to = node_2;
        strcpy(message[message_top++].text, text);
    }
    init_routing_table();

    /* first step : no changes */
    for (int i = 0; i < max_node; i++)
        routing_dijkstra(i);

    for (int i = 0; i < max_node; i++)
    {
        for (int j = 0; j < max_node; j++)
        {
            if (routing_table[i][j].cost < INF)
                fprintf(output_fd, "%d %d %d\n", j, routing_table[i][j].next, routing_table[i][j].cost);
        }
        fprintf(output_fd, "\n");
    }

    int from, to;
    for (int i = 0; i < message_top; i++)
    {
        from = message[i].from;
        to = message[i].to;

        fprintf(output_fd, "from %d to %d ", from, to);
        if (routing_table[from][to].cost >= INF)
        {
            fprintf(output_fd, "cost infinite hops unreachable message %s", message[i].text);
            continue;
        }
        fprintf(output_fd, "cost %d hops ", routing_table[from][to].cost);
        while (routing_table[from][to].cost != 0)
        {
            fprintf(output_fd, "%d ", from);
            from = routing_table[from][to].next;
        }
        fprintf(output_fd, "message %s\n", message[i].text);
    }
    fprintf(output_fd, "\n");

    /* next steps : adopting changes */
    while (fscanf(changes_fd, "%d %d %d", &node_1, &node_2, &cost) != EOF)
    {
        if (cost == -999)
            cost = INF;
        graph[node_1][node_2] = cost;
        graph[node_2][node_1] = cost;
        init_routing_table();

        for (int i = 0; i < max_node; i++)
            routing_dijkstra(i);

        for (int i = 0; i < max_node; i++)
        {
            for (int j = 0; j < max_node; j++)
            {
                if (routing_table[i][j].cost < INF)
                    fprintf(output_fd, "%d %d %d\n", j, routing_table[i][j].next, routing_table[i][j].cost);
            }
            fprintf(output_fd, "\n");
        }

        for (int i = 0; i < message_top; i++)
        {
            from = message[i].from;
            to = message[i].to;

            fprintf(output_fd, "from %d to %d ", from, to);
            if (routing_table[from][to].cost >= INF)
            {
                fprintf(output_fd, "cost infinite hops unreachable message %s", message[i].text);
                continue;
            }
            fprintf(output_fd, "cost %d hops ", routing_table[from][to].cost);
            while (routing_table[from][to].cost != 0)
            {
                fprintf(output_fd, "%d ", from);
                from = routing_table[from][to].next;
            }
            fprintf(output_fd, "message %s\n", message[i].text);
        }
        fprintf(output_fd, "\n");
    }

    printf("Complete. Output file written to output_ls.txt.\n");

    fclose(topology_fd);
    fclose(messages_fd);
    fclose(changes_fd);
    fclose(output_fd);
    return 0;
}

void init_graph()
{
    for (int i = 0; i < MAX_NODE; i++)
    {
        for (int j = 0; j < MAX_NODE; j++)
        {
            graph[i][j] = INF;
            if (i == j)
                graph[i][j] = 0;
        }
    }

    return;
}

void init_routing_table()
{
    for (int i = 0; i < MAX_NODE; i++)
    {
        for (int j = 0; j < MAX_NODE; j++)
        {
            routing_table[i][j].next = (graph[i][j] < INF) ? j : -1;
            routing_table[i][j].cost = graph[i][j];
        }
    }

    return;
}

void routing_dijkstra(int start)
{
    int D[MAX_NODE], P[MAX_NODE], visited[MAX_NODE] = {0};
    int min = INF, min_idx = -1;

    for (int i = 0; i < MAX_NODE; i++)
    {
        D[i] = INF;
        P[i] = -1;
    }

    D[start] = 0;
    P[start] = start;
    visited[start] = 1;
    int cur = start;

    for (int i = 0; i < max_node - 1; i++)
    {
        min = INF;
        min_idx = -1;

        for (int j = 0; j < max_node; j++)
        {
            if (0 < graph[cur][j] && graph[cur][j] < INF)
            {
                if (D[j] > D[cur] + graph[cur][j])
                {
                    D[j] = D[cur] + graph[cur][j];
                    P[j] = cur;
                }
            }
        }

        // for(int j=max_node-1;j>=0;j--){
        for (int j = 0; j < max_node; j++)
        {
            if (D[j] < min && visited[j] == 0)
            {
                min = D[j];
                min_idx = j;
            }
        }

        visited[min_idx] = 1;
        cur = min_idx;
    }

    for (int i = 0; i < max_node; i++)
    {
        if (i == start)
            continue;

        int parent = P[i], prev;
        routing_table[start][i].cost = D[i];
        if (parent == start)
        {
            routing_table[start][i].next = i;
            continue;
        }
        while (D[parent] != 0)
        {
            prev = parent;
            parent = P[parent];
        }
        routing_table[start][i].next = prev;
    }

    return;
}