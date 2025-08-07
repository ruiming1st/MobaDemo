# MobaDemo
This project is a multiplayer online battle arena (MOBA) game featuring real-time battles, developed using the Unreal Engine. It allows players to log in and join matches, view other players’ character attributes and inventories in real time, and check both teams’ item usage and kill information during the battle.

## Features
### Server Deployment
<p>
  <img width="500" height="684" alt="服务器运行" src="https://github.com/user-attachments/assets/fb8232ae-124f-4aab-99d5-54eaaff2b02b" />
</p>



<p>
  <img width="500" height="540" alt="玩家登陆" src="https://github.com/user-attachments/assets/05be804c-08de-4926-84a9-89addfe9b271" />
</p>

### Combat and Item System
Built upon Unreal Engine’s Gameplay framework, this project implements core gameplay mechanics including character control, progression systems, and combat logic. Hero abilities are fully designed with custom visual and gameplay effects. Both the in-game shop and inventory system provide detailed descriptions for all items.

<p>
  <img width="500" height="400" alt="商城购买" src="https://github.com/user-attachments/assets/5a3d7a12-7a69-4d99-ae41-1342a7b08030" />
</p>
<p>
  <img width="500" height="400" alt="image" src="https://github.com/user-attachments/assets/ebf955b5-dfef-43a2-be65-cc5998c56e9a" />
</p>
<p>
  <img width="500" height="400" alt="击杀播报，死亡画面，复活CD" src="https://github.com/user-attachments/assets/fab0b0c7-031b-452e-8863-bec52cbec8d9" />
</p>

### State Synchronization
The game adopts a state synchronization approach, leveraging Unreal Engine’s client-server architecture along with RPCs and property replication. The server is responsible for core gameplay logic such as damage calculation, item usage, and attribute management, while the client handles only simulation tasks—including inventory display, skill cooldowns, and buff timers—thereby reducing network traffic and improving performance.


<p>
  <img width="500" height="252" alt="buff实时CD计算" src="https://github.com/user-attachments/assets/fd3fc172-6c50-4beb-a91e-4b6834a3d021" />
</p>
<p>
  <img width="500" height="239" alt="玩家信息面板升级，技能CD" src="https://github.com/user-attachments/assets/5048c24e-d319-4333-9175-b164578f8f75" />
</p>


### Complete Gameplay Flow
character spawner is used to manage the generation of towers, crystals, minions, and jungle monsters. These entities upgrade dynamically over time as the match progresses. Towers and crystals have predefined attack priorities to control the game flow and pacing.

<p>
  <img width="500" height="352" alt="image" src="https://github.com/user-attachments/assets/1bde299f-ae4e-4de0-9ff6-1d5ef09b02a9" />
</p>

<p>
  <img width="500" height="371" alt="image" src="https://github.com/user-attachments/assets/012cdee1-0c77-4ea4-ab3a-4b58191ae985" />
</p>

<p>
  <img width="500" height="404" alt="游戏胜利" src="https://github.com/user-attachments/assets/97c2e4bc-4cbe-44ff-b2f0-c56e1b9db5a1" />
</p>










