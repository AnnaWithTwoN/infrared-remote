# Infrared remote

## EEPROM usage

### Storing a command

#### Signature

```
uint8_t eeprom_store_command (int8_t index, char* name, uint16_t* ir);
```
### Description
Stores a command on a given index or on the first empty index if -1 for index was sent.
#### Parameters
| name  | description |
| ------------- | ------------- |
| index  | index where to store this command; use -1 for any  |
| name  | name of the command  |
| ir  | array of recorded edge timings  |

#### Example usage
```
uint16_t ir[250];
char* name = "hey you";
ir[0] = 200;
ir[1] = 201;
ir[2] = 202;
eeprom_store_command(-1, name, ir);
```

### Loading a command

#### Signature

```
uint8_t eeprom_load_command (uint8_t index, uint16_t * ir); 
```
#### Parameters
| name  | description |
| ------------- | ------------- |
| index  | index of the command to be loaded |
| ir  | pointer to the array of recorded edge timings, where result will be loaded |

#### Example usage
```
uint16_t ir[250];
eeprom_load_command(0, ir);
print_command(ir);
```

### Navigating through commands

#### Signature

```
int8_t eeprom_get_prev_command(int8_t* current_index, char* name);
int8_t eeprom_get_next_command(int8_t* current_index, char* name);
```
### Description
Function returns index and a name of the previous or next command from the memory. It takes pointer to an index and starts searching prev/next command from that index, or from 0, if current_index has value of -1. The searching is circular, meaning it the next command for index 64 (max index) will be 0, the previous command for index 0 will be 64, so you don't have to worry about out-of-range indexes. If no any stored command found, -1 is returned, else the current index is set to found command and into name is loaded found command's name.
### Note 
Commands are stored at random indexes to improve efficiency (for example, commands might be stored at indexes 1, 2 and 5, but indexes 0, 3, 4 and others will be empty), so manually incrementing index and loading commands will not result in desired outcome. 
#### Parameters
| name  | description |
| ------------- | ------------- |
| current_index  | pointer to an index; send -1 to denote start searching from the beggining |
| name  | pointer to an array where command name will be loaded |

#### Example usage
```
char name[10];
int8_t current_index = -1;
int8_t res = eeprom_get_next_command(&current_index, name);
if (res == -1) {
    // handle case when no commands are found
} else {
    uart_sendstring(name);
    uart_sendstring(i16tos(current_index));
}
```

### Deleting a command

#### Signature

```
uint8_t eeprom_delete_command (uint8_t index);
```
#### Parameters
| name  | description |
| ------------- | ------------- |
| index  | index of the command to be deleted |

#### Example usage
```
eeprom_delete_command(1);
```