#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <string> 
#include <stdio.h>

#define BLOCK_NAME_LENGTH 5
#define COMMAND_LENGTH  100
#define NODENAME_LENGTH 5

using namespace std;

int main()
{
    int k_before = 2;
    int m = 2;
    int k_after = 5;

    char stripe_layout[k_before+m][BLOCK_NAME_LENGTH] = {};
    char group_layout[(k_before+m)*(k_before+m)][BLOCK_NAME_LENGTH] = {};
    char temp[BLOCK_NAME_LENGTH];

    int i,j,k;
    int stripe_length = k_before + m;
    int n, s;
    int pg, dg;
    int pg_stripe, dg_stripe;
    int test_stripe_num = 1;
    int start_send_parity_index = 0;

    n = k_before + m;
    s = k_after - k_before;
    pg_stripe = n * k_before * (n+s);
    dg_stripe = n * s * (n+s);
    pg = pg_stripe / n;
    dg = dg_stripe / n;
    start_send_parity_index = n * k_before * (n - s * (n-k_before-1));
    
    cout << "pg=" << pg_stripe << "dg=" << dg_stripe << endl;
    cout << start_send_parity_index << endl;

    char total_layout[n * (pg_stripe+dg_stripe)][BLOCK_NAME_LENGTH] = {};
    char dg_layout[n * dg_stripe][BLOCK_NAME_LENGTH] = {};
    char dg_datablock[stripe_length * dg * k_before][BLOCK_NAME_LENGTH] = {};
 
    for (i = 0; i < k_before+m; i++)
    {
        /* code */
        if (i < k_before)
        {
            /* code */
            strcpy(stripe_layout[i], "P");
        }
        if (i >= k_before)
        {
            /* code */
            strcpy(stripe_layout[i], "D");
        }
    }
    
    for (i = 0; i < (k_before+m); i++)
    {
        /* code */
        for (j = 0; j < k_before+m; j++)
        {
            /* code */
            strcpy(group_layout[j+i*stripe_length], stripe_layout[j]);
        }

        strcpy(temp, stripe_layout[stripe_length-1]);

        for (k = stripe_length-2; k >= 0; k--)
        {
            /* code */
            strcpy(stripe_layout[k+1], stripe_layout[k]);
        }

        strcpy(stripe_layout[0], temp);
    }

    for (i = 0; i < pg+dg; i++)
    {
        /* code */
        for (j = 0; j < n*stripe_length; j++)
        {
            /* code */
            strcpy(total_layout[j+i*n*n], group_layout[j]);
        }
        
    }
    
    int d_index = 0;
    int p_index = 0;
    string temp_block;
    for (i = 0; i < (pg_stripe+dg_stripe) * stripe_length; i++)
    {
        /* code */
        if (strcmp(total_layout[i], "D") == 0)
        {
            /* code */
            temp_block = "D" + to_string(d_index);
            strcpy(total_layout[i], temp_block.c_str());
            d_index++;
        }

        if (strcmp(total_layout[i], "P") == 0)
        {
            /* code */
            temp_block = "P" + to_string(p_index);
            strcpy(total_layout[i], temp_block.c_str());
            p_index++;
        }
        
    }

    for (i = pg_stripe * stripe_length; i < (pg_stripe+dg_stripe) * stripe_length; i++)
    {
        /* code */
        strcpy(dg_layout[i - pg_stripe * stripe_length], total_layout[i]);
    }

    int datablock_index = 0;
    for (i = 0; i < n; i++)
    //n???
    {
        /* code */
        datablock_index = 0;
        for (j = 0; j < dg_stripe*stripe_length; j+=stripe_length)
        //????????????????????????dg_stripe????????????dg???
        {
            /* code */
            if (strstr(dg_layout[i+j], "D") != 0)
            //?????????????????????????????????+index?????????dg_datablock????????????
            {
                /* code */
                strcpy(dg_datablock[datablock_index+i], dg_layout[j+i]);
                datablock_index += stripe_length;
                //????????????
            }
            else
            {
                continue;
            }
        }
    }
    
    cout << "Total layout:" << endl;
    for (i = 0; i < (pg_stripe+dg_stripe) * stripe_length; i++)
    {
        /* code */
        if (i!=0 && i%stripe_length ==0)
        {
            /* code */
            cout << endl;
            test_stripe_num ++;
        }
        
        cout << total_layout[i] << " ";

    }
    cout << endl;

    cout << "DG laytout:" << endl;

    for (i = 0; i < dg_stripe * stripe_length; i++)
    {
        /* code */
        if (i!=0 && i%stripe_length == 0)
        {
            /* code */
            cout << endl;
        }
        
        cout << dg_layout[i] << " ";
    }

    cout << endl;
    cout << "DG datablock layout:" << endl;
    for (i = 0; i < dg * k_before * stripe_length; i++)
    {
        /* code */
        if (i!=0 && i%stripe_length ==0)
        {
            /* code */
            cout << endl;
        }
        
        cout << dg_datablock[i] << " ";
    }

    
    char pg_transferparity[((pg_stripe - start_send_parity_index) / n) * stripe_length][BLOCK_NAME_LENGTH] = {};

    //????????????????????????datanode???parity
    int transfer_parity_index = 0;
    int select_parity_row = 0;
    for (i = start_send_parity_index; i < pg_stripe; i++)
    {
        /* code */
        //????????????????????????parity
        for (j = select_parity_row%stripe_length; j < stripe_length; j++)
        {
            /* code */
            if (strstr(total_layout[j+i*stripe_length], "P") != 0)
            {
                /* code */
                strcpy(pg_transferparity[transfer_parity_index], total_layout[j+i*stripe_length]);
                transfer_parity_index ++;
                select_parity_row ++;
                break;
            }
            else
            {
                continue;
            }
               
        }
    }

    cout << endl;
    cout << "Transfer parity block layout:" << endl;
    for (i = 0; i < ((pg_stripe - start_send_parity_index) / n) * stripe_length; i++)
    {
        /* code */
        if (i!=0 && i%stripe_length == 0)
        {
            /* code */
            cout << endl;
        }
        
        cout << pg_transferparity[i] << " ";
    }
    
    char dg_transferblock[n * dg * k_before][BLOCK_NAME_LENGTH] = {};

    //???????????????????????????????????????transferblock?????????
    for (i = 0; i < (start_send_parity_index / n) * s; i++)
    {
        /* code */
        for (j = 0; j < stripe_length; j++)
        {
            /* code */
            strcpy(dg_transferblock[j+i*stripe_length], dg_datablock[j+i*stripe_length]);
        }
        
    }
    

    //????????????parity???DG??????datablock??????
    int start_exchange_index = 0;
    int transferparity_index = 0;
    int start_index = ((start_send_parity_index / n) * s); //dg???????????????????????????????????????
    for (i = start_index; i < dg * k_before; i++)
    {
        /* code */
        if (i%s == 0 && i != start_index)
        {
            /* code */
            //?????????DG????????????????????????????????????????????????s?????????0????????????
            start_exchange_index++;
            if (start_exchange_index % s == 0)
            {
                /* code */
                start_exchange_index = 0;
            }
        }
        
        if (i%s == start_exchange_index)
        {
            /* code */
            //??????datablock???parity
            for (j = 0; j < stripe_length; j++)
            {
                /* code */
                strcpy(dg_transferblock[j+i*stripe_length], pg_transferparity[j+transferparity_index*stripe_length]);
            }
            transferparity_index++;
        }

        else if (i%s != start_exchange_index)
        {
            /* code */
            //???????????????
            for (j = 0; j < stripe_length; j++)
            {
                /* code */
                strcpy(dg_transferblock[j+i*stripe_length], dg_datablock[j+i*stripe_length]);
            }
        }
      

    }
    
    cout << endl;
    cout << "Transfer block layout:" << endl;
    for (i = 0; i < dg * k_before * stripe_length; i++)
    {
        /* code */
        if (i!=0 && i%stripe_length ==0)
        {
            /* code */
            cout << endl;
        }
        
        cout << dg_transferblock[i] << " ";
    }

    //??????????????????
    //?????????????????????_???????????????DM/UP???_????????????_????????????_??????
    //dg??????????????????s????????????UP??????m-1???datanode
    
    int round_index = 0;
    string data_migration = "DM";
    string update_parity = "UP";
    char node_set[k_after+m][NODENAME_LENGTH] = {}; //0???namenode??????1?????????datanode
    
    string nodename;
 
    //????????????????????????
    for (i = 0; i <= k_after+m; i++)
    {
        /* code */
        nodename = "N" + to_string(i);
        strcpy(node_set[i], nodename.c_str());
    }
    
    cout << endl;
    cout << "Node set:" << endl;
    for (i = 0; i < k_after+m+1; i++)
    {
        /* code */
        cout << node_set[i] << " ";
    }
    
    
    char up_datablock_group[k_before*(n+s) * n][s*BLOCK_NAME_LENGTH] = {}; //dg*k/s == s*(n+s)*k/s = k*(n+s)

    string datablock_group;
    string datablock_temp;

    //???????????????????????????
    for (i = 0; i < k_before*(n+s); i++)
    {
        /* code */
        for (j = 0; j < n; j++)
        {
            /* code */
            datablock_group.clear();
            for (k = 0; k < s*stripe_length; k+=stripe_length)
            {
                /* code */
                //???j?????????????????????
                datablock_temp.clear();
                datablock_temp = dg_datablock[k+j+i*s*n];
                datablock_group = datablock_group + datablock_temp;
            } 
 
            strcpy(up_datablock_group[j+i*n], datablock_group.c_str());
        }
    }

    // cout << endl;
    // cout << "Update parity data block group:" << endl;
    // for (i = 0; i < k_before*(n+s)*n; i++)
    // {
    //     /* code */
    //     if (i!=0 && i%n ==0)
    //     {
    //         /* code */
    //         cout << endl;
    //     }
        
    //     cout << up_datablock_group[i] << " ";
    // }   

    //??????dg_datablock??????update parity??????
    //?????????????????????_???????????????DM/UP???_????????????_????????????_??????
    char up_command[k_before*(n+s) * (m-1) * n][COMMAND_LENGTH] = {};
    int up_round_number = 0;
    string command;
    string command_gap = "_";

    string send_node;
    string recv_node;
    //?????????
    for (i = 0; i < k_before*(n+s); i++)
    {
        /* code */
        //???????????????m-1???datanode
        for (j = 0; j < m-1; j++)
        {
            /* code */
            //n???datanode
            for (k = 0; k < n; k++)
            {
                /* code */
                send_node.clear();
                recv_node.clear();
                datablock_temp.clear();
                if (k+1+j+1 > n)
                {
                    /* code */
                    send_node = node_set[k+1];
                    recv_node = node_set[k+1 + j+1 - n];
                    datablock_temp = up_datablock_group[k+i*n];
                    command = to_string(j+1+i*(m-1)) + command_gap + update_parity + command_gap + send_node + command_gap + recv_node + command_gap + datablock_temp;
                    strcpy(up_command[k + j*n + i*(m-1)*n], command.c_str());
                }
                else
                {
                    send_node = node_set[k+1];
                    recv_node = node_set[k+1 + j+1];
                    datablock_temp = up_datablock_group[k+i*n];
                    command = to_string(j+1+i*(m-1)) + command_gap + update_parity + command_gap + send_node + command_gap + recv_node + command_gap + datablock_temp;
                    strcpy(up_command[k + j*n + i*(m-1)*n], command.c_str());
                }
            } 
            up_round_number++; 
        }   
    }

    
    cout << endl;
    cout << "Need " << up_round_number << " round to update parity" << endl;
    cout << "Update parity command:" << endl;
    for (i = 0; i < k_before*(n+s) * (m-1) * n; i++)
    {
        /* code */
        if (i!=0 && i%n ==0)
        {
            /* code */
            cout << endl;
        }
        
        cout << up_command[i] << " ";
    }   

    
    //??????transferblock??????data migration??????
    char dm_command[dg * k_before * n][COMMAND_LENGTH] = {};

    command.clear();
    send_node.clear();
    recv_node.clear();
    
    for (i = 0; i < (n+s) * k_before; i++)
    {
        /* code */ 
        //0~n-1????????????
        for (j = 0; j < n; j++)
        {
            /* code */
            for (k = 0; k < s; k++)
            {
                /* code */
                if (k+j >= n)
                {
                    /* code */
                    send_node = node_set[k+1+j-n];
                    recv_node = node_set[n+k+1];
                    datablock_temp = dg_transferblock[k+j-n + k*stripe_length + i*s*stripe_length];
                    command = to_string(j+1+i*n+up_round_number) + command_gap + data_migration + command_gap + send_node + command_gap + recv_node + command_gap + datablock_temp;
                    strcpy(dm_command[k + j*s + i*s*n], command.c_str());
                }
                else if(k+j < n)
                {
                    send_node = node_set[k+1+j];
                    recv_node = node_set[n+k+1];
                    datablock_temp = dg_transferblock[k+j+k*stripe_length + i*s*stripe_length];
                    command = to_string(j+1+i*n+up_round_number) + command_gap + data_migration + command_gap + send_node + command_gap + recv_node + command_gap + datablock_temp;
                    strcpy(dm_command[k + j*s + i*s*n], command.c_str());
                }
            }
            
        }
        

    }

    cout << endl;
    cout << "Data migration command:" << endl;
    for (i = 0; i < dg * k_before * n; i++)
    {
        /* code */
        if (i!=0 && i%s ==0)
        {
            /* code */
            cout << endl;
        }
        
        cout << dm_command[i] << " ";
    }   
}
