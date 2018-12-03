#include <stdio.h>
#include <ctype.h>
#include "token.h"




void generator_init();
void print_output();
void gen_header();
void gen_main_start(); 
void gen_main_end();
void gen_def_start(char *f_name);
void gen_def_end();
void gen_var_from_token(token_t token);
void gen_frame_retvar();
void switch_stack();
void gen_less_or_equal();
void gen_more_or_equal();






void pomocna_docasna_funkce();




























/*
_____.___.           /\              ___.                            ________                                 .___
\__  |   | ____  __ _)/__  __ ____   \_ |__   ____   ____   ____    /  _____/  ____   ____   _____   ____   __| _/
 /   |   |/  _ \|  |  \  \/ // __ \   | __ \_/ __ \_/ __ \ /    \  /   \  ___ /    \ /  _ \ /     \_/ __ \ / __ | 
 \____   (  <_> )  |  /\   /\  ___/   | \_\ \  ___/\  ___/|   |  \ \    \_\  \   |  (  <_> )  Y Y  \  ___// /_/ | 
 / ______|\____/|____/  \_/  \___  >  |___  /\___  >\___  >___|  /  \______  /___|  /\____/|__|_|  /\___  >____ | 
 \/                              \/       \/     \/     \/     \/          \/     \/             \/     \/     \/ 


                                             ```.....,,,:,.``                                                                                                          
                                         ``.:'++++++++++++++;.`                                                                                                        
                                        `:'+++++++'+++++++++++;::.`                                                                                                    
                                      `:++++++++++++++++++++++++++:                                                                                                    
                                     :'++++++++++++++++++++++++++++,                                                                                                   
                                   `:++++++++++++++++++++++++++++++'```                                                                                                
                                  `;++++++++++++++++++++++++++++++++;.`                                                                                                
                                  :++++++++++++++++++++++++++++++++++++``                                                                                              
                                 `'++++++++++++++++++++++++++++++++++++'.                                                                                              
                                 .+'++++++++++++++++++++++++++++++++++++'`                                                                                             
                                 ;+++++++++++++++++++++++++++++++++++++'+.                                                                                             
                                ;++++++++++++++++++++++++++++++++++++++++,`                                                                                            
                                +'+++++++++++++++++++++++++++++++++++++++;`                                                                                            
                               `++++++++++++++++++++++++++++++++++++++++++.                                                                                            
                               `#+++++++++++++++++++++++++++++++++++++++++'``                                                                                          
                               :++++++++++++++++++++++++++++++++++++++++++++:`                                                                                         
                              `'++++++++++++++++++++++++++++++++++++++++++++#:`                                                                                        
                              ,+++++++++++++++++++++++++++++++++++++++++++++++.                                                                                        
                             `'+++++++++++++++++++++++++++++++++++++++++++++++:                                                                                        
                             :'+++++++++++++++++++++++++++++++++++++++++++++++:                                                                                        
                            ,+++++++++++++++';;;;;;;;;;;;;;;;;'''''+++++++++++:                                                                                        
                            :+++++++++';:,.....,.............,,,,,:...,,,:;'++`                                                                                        
                            :+++++++;:,........,,,,,....,,,,,,,,,,,,,,,,,,,,;'                                                                                         
                            ;+##++#'::,........,,...........,,,,,,,,.........`                                                                                         
                           `,:;'+#+:,:,...........,.,,,,,.,,,,,,,,,,,,,....,.`                                                                                         
                           `..,,:'+.,,,..........,,,,,,,,,,,,,,,,,,,,,,,,,,,,                                                                                          
                           `,,..,,:..,,,.........,,,,,,,,,,,,,,,,,,:,,,,,,,,.                                                                                          
                           `:::,,,,...,,,.......,.,,::::::,,,,,,,::::,,,,,,,`                                                                                          
                           `:::::,,.....,.......,,::''++;;;:,,,,:'+++:,,,,,,.`                                                                                         
                           `,::;;;,.....,.......,,:;#+#'+;';:,,,;###''::,,.,.`                                                                                         
                            .:::;;:,....,,.....,,,:'#+##+;':,,,,:###+':,,,.,.`                                                                                         
                             ,::;:,,....,,.....,,,,,'+##';:::,,,::'++;:,,,.,.`                                                                                         
                             .::;:,:,...,,.....,,,,,,,,,,,,:,::,:,,,,,:,,,.,.`                                                                                         
                              ,,::,:,..,,,.,.,,,,,,,,,,,,,,,,:,:,,,,,,,,,....`                                                                                         
                              .,,,,:,..,,,..,,,,,,,,,,,,,,,::::,,,,,,,,,,,.,.`                                                                                         
                              `,,,,::..,,,...,,,,,,,,,,,,,,::::,,,,,,,,,,,...`                                                                                         
                               .,,,:;..,,,...,,,,,,,,,,,,,,::,,,,,,,,,,,,,...`                                                                                         
                               .,,,::...,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,...                                                                                         
                               `,,::,...,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,....                                                                                         
                                .,:,,...,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,.,`                                                                                        
                                .,:,,...,,,,,,,,,,,,,,,,,,:,,,,,,,,,,,,,,,...,`                                                                                        
                                 ,;:,......,,,,,,,,,,,,::::::,,::::,,,,,,......                                                                                        
                                 `;',.......,::,,,,.,,,,,,,:;';;;;;,,,.........                                                                                        
                                .'#+..........,,,,,,.,,,.,,,,,,:,,,,...........                                                                                        
                               ,###+........,.,,.,,,,,,,,,,,,,,,,...........,..                                                                                        
                              ;####'............,,,,,,,,,,,,,,,,,:::,,.......,.                                                                                        
                            `+##++++............,,,,,::::;'+++++''';:,.......,.                                                                                        
                           `+##+++++,...........,,,,,,,,,::;;;;;;;::,........,.                                                                                        
                          `'#+++++++'...,.......,,,,,,,,::::::,,::::...........                                                                                        
                         `####++++++;.............,,,,,,:::::::;:,.............                                                                                        
                       ` '######+++++,.,...........,,,,,,,,,,,,,,,,,,,,......,.                                                                                        
                        ;########+++#;.,............,,,,,,,,,,,,,,,,,,,......,`                                                                                        
                       :#########+++++,.............,,,,,,,,,,,,,,,,,,,.......`                                                                                        
                      :#+########++++++:,.....................................`                                                                                        
                     :#+#########+++++++......................................`                                                                                        
                    `+############++++++:.,...................................;`                                                                                       
                   `'#############+++++#+,...................................,##:.``.,,,.                                                                              
                   ;#+############+++++++'...................................:###########'                                                                             
                  .++#+###########++++++#+,..................................;#########+##.                                                                            
                  ;+++++##########+++++++#:................................`;######++++++#,                                                                            
                 `'+++++##########+++++++++...............................`.+#####++++++##'       ```.`                                                                
                 `+++++++#########++++++++++:.............................`,######++++++''+.   `.,,,:::,.`                                                             
                 ,+++++++#########+++++++++++:.............................;######++++';:,,,::,,,,,,,,:;;::.`                                                          
                 '+++++++++#######+++++++++++;`...........................'#######++++;;::,,,,,,,,,,::,:;;;;:.`                                                        
                :+++++++++########++++++++++++:...........................+#######++++;::,,,,,,,,,,,,;;;::::;;,`                                                       
               .#+++++###+########++++++++++++###',......................,+#######++++;::,,,,:::::::,,::;;;::::`                                                       
               '++#+'''+##########+++++++++++++++##;.,................,.,;########++++;:,,,:,::::,;'::,:::;;;:,`                                                       
              .+#+#+;;::;+#########+++++++++++++++#+++##':...........;+++#########++++;,,,,,,,::,,,;'';;;:,:;;:,`                                                      
              ,+###';:,:::;;++;;;'++++'+++++++++++++++++#+:.```.``.:+++++++#######+++++:,,,,,:::,:,,:;';;;:,,,::`                                                      
              .####;::::::::::,::,,,,,,:'+++++++++++++++++++###+++#++++++++#######++++###';,,::::,,,,,,::::;:,,,`                                                      
              `####;,,,,,::::::::::::::,:#++++++++++++++++++++++++++++++++++#######+++####+``,,,,,,,,,,,,,,.,:::.                                                      
              `####;,,:,,,:::::::::::::.:#+++++++++++++++++++++++++++++++++++######+++####`  `,,:,,,,,,,,,,,.`,,.                                                      
               +###',,,,,::::::::::;++++++++++++++++++++++++++++++++++++++++########+++##+`   .:,,.```..,,,,,``                                                        
               +#+#',,,,,:,,,:,:::::'##+++++++++++++++++++++++++++++++++++++:###########'.     .::,`     .,,,                                                          
               '#++',,,,,,,::,,,:::::;'#++++++++++++++++++++++++++++++++++++ .+##+#####+`      `,::.      ```                                                          
               '#++',,,,,,,:::;;:::::,:;'+++++++++++++++++++++++++++++++++++   `,'#####,        `:,:`                                                                  
               :##++,,,,,,:::::;'';::::::,'#++++++++++++++++++++++++++++++++       ``.`          `:,:`                                                                 
               ,###+:,,,,,,:::::;''+;::::::++++++++++++++++++++++++++++++++'                      `,::.                                                                
               .####',,,,,,:::,:::;+##+':,:+++++++++++++++++++++++++++++++#:                       `,,,                                                                
                '####::,,,,,:::,,::;'###+++++++++++++++++++++++++++++++++++`                        `,,                                                                
                `;':.`,,,,,,:;;;:::,:;+##+++++++++++++++++++++++++++++++++:                          ``                                                                
                      `:,:,,,,;''':,,,:::;+++++++++++++++++++++++++++++++'`                                                                                            
                       .:,:::::;''';,:::::##+###++++++++++++++++++++++++#.                                                                                             
                        ,,::;;:::;'+';:::'#######++++++++++++++++++++++',                                                                                              
                        `:::;'';:::;+######+######+++++++++++++++++++++`                                                                                               
                         `,::;'+';:::;++#++++++++++++++++++++++++++++++  `                                                                                             
                           `,::''++::::;#++++++++++++++++++++++++++++++.                                                                                               
                            ,:,:;;++;,:,++++++++++++++++++++++++++++++#,                                                                                               
                            ;..;,:;'#;,:#++++++++++++++++++++++++++++++:                                                                                               
                           `' .#+:,:;++++++++++++++++++++++++++++++++++'                                                                                               
                           `, ,#++:,:++++++++++++++++++++++++++++++++++'                                                                                               
                           ,. ;#+##+++++++++++++++++++++++++++++++++++++`                                                                                              
                           ;  ++++#++++++++++++++++++++++++++++++++++++#`                                                                                              
                           ;  '++++++++++++++++++++++++++++++++++++++++#`                                                                                              
                           `  ;++++++++++++++++++++++++++++++++++++++#;.                                                                                               
                              ;#+#++++++++++++++++++++++++++++++++++++                                                                                                 
                              `'#+###++++++++++++#+####++++++++++++++#`                                                                                                
                               `+++++++++++++++++++++#####+##++++++++;                                                                                                 
                                ,#+++++++++++++++#++#+#########++++++,                                                                                                 
                                 '++++++++++++++++#',@#########+++++#.                                                                                                 
                                 .+++++++++++++++##, '#########+++++#`                                                                                                 
                                  ++++++++++++++++#. :#########++++++                                                                                                  
                                  :++++++++++++++++  ,#########++++#;                                                                                                  
                                  `+++++++++++++++'  `########++++++:                                                                                                  
                                   +++++++++++++++,  `########++++++,                                                                                                  
                                   '+++++++++++++#, `,########+++++#.                                                                                                  
                                   '+++++++++++++#. `'+#######++++++`                                                                                                  
                                   '++++++++++++#+..##########+++++; `                                                                                                 
                                  `+++++++++++++++:@##########+++++:                                                                                                   
                                  :#++++++++++++++ @##########++++++, `                                                                                                
                                '+#+++++++++++++#+ +#########+++#####;                                                                                                 
                                '##+++++++++++++++:.@@##############@.                                                                                                 
                                ,@#######+++++++##@#@@@@###########@'                                                                                                  
                                `@#################''@@#############.                                                                                                  
                                 '################@,,@@@###########'                                                                                                   
                                 ,@###############+` ##############.                                                                                                   
                                 `@##############@:  +##########@#'                                                                                                    
                                  +###############`  :@@#######@##:                                                                                                    
                                  '#############@'```,@########@#+`                                                                                                    
                          ````````;@##########@##.```.@#########@;                                                                                                     
                        `````......#############+.....#@########@,`````    `````                                                                                       
                        ````.......+###########@:....,#########@#````````````````                                                                                      
                        ```........'###########+....`,@##########.`````````````````                                                                                    
                       ```.........'###########;`....;@#########@;````````````````````                                                                                 
    `````````````      ```....,,,,.'###########:....;@###########@;``````````````````````                                                                              
    `````````````````````....,,,,,.;@##########.`...##@@########@##'``.`````````````````````````                                                                       
   `````````````````````....,,,,,,.'###########....,#@###############',`.`````````````````````````````                                                                 
   `````````````````````....,,,,,,:############+...,@##############@#@##:``.````````````````````````````                                                               
   ````````````````````.....,,,,,,###############,,.@#################@###,`.````````````````````````````                                                              
   `````````................,,,,,:#############@##,.#@####################+`..````````````````````````````                                                             
   ```...........................;###############@#'#@####################@.``...........````````````````                                                              
   ```...........................;################@@#@####################@:.`...........`````````````````                                                             
 ````.........................``.:##@###########@##########################'``...........`````````````````                                                             
````..........................```.@#####################@#@@################`............```````````````````````                                                       
````..........................```.@#####################+..,###############@`............````````````````````````                                                      
````..........................```.#######################.``,#@#############`............````````````````````````                                                      
`````.........................````######################@,`.`.############@#`......,,,...````````````````````````                                                      
`````````.....................````,'#@@##################;``.`.,:'+@@@@@#;,`......,,,....````````````````````````                                                      
`````````.....................``````..;+@################+```..``````...```.`.....,,,,...````````````````````````                                                      
`````````.....................```````.`.#################+```.`````````..``.......,,,,...````````````````````````                                                      
`````````.....................````````.`:#@##############'`..`````.`````````..............```````````````````````                                                      
`````````.....................```````````.'#@##########@#:``````````````````..............```````````````````````                                                      
``````````....................`````````````.,;########+:.```````````````````.............````````````````````````                                                      
```````````.........`````````````````````.``...```````.``````````````````````````````````````````````````````````                                                      
         ````````````````````````````````````````````````````````````````````````````````````````````````````````                                                      
          ```````````````````````````````````````````````````````````````````````````````       ````````````                                                           
            ``````````````````````````````````````````````````````````````````````````                     
*/