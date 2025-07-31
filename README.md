# MobaDemo
This project is a multiplayer online battle arena (MOBA) game featuring real-time battles, developed using the Unreal Engine. It allows players to log in and join matches, view other players’ character attributes and inventories in real time, and check both teams’ item usage and kill information during the battle.

## Features
### Server Deployment
<img width="584" height="387" alt="image" src="https://github.com/user-attachments/assets/65791f30-4c3b-4c12-b9cd-94ccd5bd6f42" /><img width="608" height="387" alt="image" src="https://github.com/user-attachments/assets/60f8b281-19eb-4dd4-8be3-ccd9754372b6" /> 

### 
Built upon Unreal Engine’s Gameplay framework, this project implements core gameplay mechanics including character control, progression systems, and combat logic. Hero abilities are fully designed with custom visual and gameplay effects. Both the in-game shop and inventory system provide detailed descriptions for all items.

###
The game adopts a state synchronization approach, leveraging Unreal Engine’s client-server architecture along with RPCs and property replication. The server is responsible for core gameplay logic such as damage calculation, item usage, and attribute management, while the client handles only simulation tasks—including inventory display, skill cooldowns, and buff timers—thereby reducing network traffic and improving performance.



