//Animation ID ranges
#define DAMAGE_START 0x4B
#define DAMAGE_END 0x5B
#define CAPTURE_START 0xDF
#define CAPTURE_END 0xE8
#define GUARD_START 0xB2
#define GUARD_END 0xB6
#define GROUNDED_CONTROL_START 0xE
#define GROUNDED_CONTROL_END 0x18
#define TECH_START 0xC7
#define TECH_END 0xCC

//Animation ID specific
#define ROLL_FORWARD 0xE9
#define ROLL_BACKWARD 0xEA
#define SPOT_DODGE 0xEB
#define AIR_DODGE 0xEC
#define ACTION_WAIT 0xE
#define ACTION_DASH 0x14
#define ACTION_KNEE_BEND 0x18
#define GUARD_ON 0xB2
#define TECH_MISS_UP 0xB7
#define TECH_MISS_DOWN 0xBF

//Stage IDs
#define STAGE_FOD 2
#define STAGE_POKEMON 3
#define STAGE_YOSHIS 8
#define STAGE_DREAM_LAND 28
#define STAGE_BATTLEFIELD 31
#define STAGE_FD 32

char* externalCharacterNames[] = {
  "Captain Falcon",
  "Donkey Kong",
  "Fox",
  "Mr. Game & Watch",
  "Kirby",
  "Bowser",
  "Link",
  "Luigi",
  "Mario",
  "Marth",
  "Mewtwo",
  "Ness",
  "Peach",
  "Pikachu",
  "Ice Climbers",
  "Jigglypuff",
  "Samus",
  "Yoshi",
  "Zelda",
  "Sheik",
  "Falco",
  "Young Link",
  "Dr. Mario",
  "Roy",
  "Pichu",
  "Ganondorf",
  "Master Hand",
  "Wireframe Male (Boy)",
  "Wireframe Female (Girl)",
  "Giga Bowser",
  "Crazy Hand",
  "Sandbag",
  "Popo",
  "User Select(Event) / None"
};

char* colors[] = {
  "Default",
  "Red",
  "Blue",
  "Green",
  "?????",
  "?????",
  "?????"
};

char* stages[] = {
  "Dummy",
  "TEST",
  "Fountain of Dreams",
  "PokÃ©mon Stadium",
  "Princess Peach's Castle",
  "Kongo Jungle",
  "Brinstar",
  "Corneria",
  "Yoshi's Story",
  "Onett",
  "Mute City",
  "Rainbow Cruise",
  "Jungle Japes",
  "Great Bay",
  "Hyrule Temple",
  "Brinstar Depths",
  "Yoshi's Island",
  "Green Greens",
  "Fourside",
  "Mushroom Kingdom I",
  "Mushroom Kingdom II",
  "Akaneia",
  "Venom",
  "PokÃ© Floats",
  "Big Blue",
  "Icicle Mountain",
  "Icetop",
  "Flat Zone",
  "Dream Land N64",
  "Yoshi's Island N64",
  "Kongo Jungle N64",
  "Battlefield",
  "Final Destination",
  "Young Link (TClink)",
  "Donkey Kong (TDonkey)",
  "Target Test Mario (TMario)",
  "Target Test C. Falcon (TCaptain)",
  "Target Test Young Link (TClink)",
  "Target Test Donkey Kong (TDonkey)",
  "Target Test Dr. Mario (TDrmario)",
  "Target Test Falco (TFalco)",
  "Target Test Fox (TFox)",
  "Target Test Ice Climbers (TIceclim)",
  "Target Test Kirby (TKirby)",
  "Target Test Bowser (TKoopa)",
  "Target Test Link (TLink)",
  "Target Test Luigi (TLuigi)",
  "Target Test Marth (TMars)",
  "Target Test Mewtwo (TMewtwo)",
  "Target Test Ness (TNess)",
  "Target Test Peach (TPeach)",
  "Target Test Pichu (TPichu)",
  "Target Test Pikachu (TPikachu)",
  "Target Test Jigglypuff (TPurin)",
  "Target Test Samus (TSamus)",
  "Target Test Sheik (TSeak)",
  "Target Test Yoshi (TYoshi",
  "Target Test Zelda (TZelda)",
  "Target Test Mr. Game & Watch (TGamewat)",
  "Target Test Roy (TEmblem)",
  "Target Test Ganondorf (TGanon)",
  "Kinoko (Mushroom Kingdom Adventure)",
  "Castle (vs Peach & Mario [or luigi])",
  "Kongo (vs 2 mini Donkey Kongs)",
  "Garden (vs Donkey Kong)",
  "Meiktu (Zelda Adventure[Underground Maze])",
  "Shrine (vs Zelda)",
  "Zebes (vs Samus)",
  "Dassyut (Escape from Brinstar Adventure)",
  "Greens (vs Kirby)",
  "Greens (vs Kirby Team)",
  "Greens (classic Kirby music) (vs Giant Kirby)",
  "Corneria (vs Fox [or Falco])",
  "Corneria (vs Fox [or Falco] with massive arwing attack)",
  "PokÃ©mon Stadium (vs Pikachu Team, and 1 Jigglypuff if unlocked, lots of pokeball)",
  "B Route (F  //Zero Adventure[F  //Zero Grand Prix])",
  "Mute City (vs Captain Falcon)",
  "Onett (vs Ness x3, lots of Mr Saturn)",
  "Icemt (Icicle Mountain Adventure)",
  "Icetop (vs Ice Climbers x2)",
  "Battle (vs Fighting Wireframe team, low gravity)",
  "Battle (vs Metal Mario [& Metal Luigi if unlocked])",
  "Last (vs Bowser)",
  "Last (vs Giga Bowser)",
  "Takisusume (Race to the Finish Classic)",
  "Grab the Trophies (figureget)",
  "Homerun Contest (homerun)",
  "Heal (All-Star's Stage Inbetween Matches)",
  "Princess Peach's Castle (vs Mario)",
  "Rainbow Cruise (vs Mario)",
  "Kongo Jungle (vs Donkey Kong)",
  "Jungle Japes  (vs Donkey Kong)",
  "Great Bay (vs Link)",
  "Temple (vs Link)",
  "Brinstar (vs Samus)",
  "Brinstar Depths (vs Samus)",
  "Yoshi's Story (vs Yoshi)",
  "Yoshi's Island (vs Yoshi)",
  "Fountain of Dreams (vs Kirby)",
  "Green Greens (vs Kirby)",
  "Corneria (vs Fox)",
  "Venom (vs Fox)",
  "PokÃ©mon Stadium (Only Pokeballs)(vs Pikachu)",
  "Mushroom Kingdom I (vs Luigi)",
  "Mushroom Kingdom II (vs Luigi)",
  "Mute City (vs Captain Falcon)",
  "Big Blue (vs Captain Falcon)",
  "Onett (vs Ness)",
  "Fourside (vs Ness)",
  "PokÃ©mon Stadium (vs Jigglypuff)",
  "Princess Peach's Castle (vs Bowser)",
  "Battlefield (vs Bowser)",
  "Princess Peach's Castle (vs Peach)",
  "Mushroom Kingdom II (vs Peach)",
  "Temple (vs Zelda)",
  "Great Bay (vs Marth)",
  "Final Destination (vs Mewtwo)",
  "PokÃ©mon Stadium (vs Mewtwo)",
  "Icicle Mountain (vs Ice Climbers)",
  "Icicle Mountain (vs Ice Climbers)",
  "Mushroom Kingdom I (Dr. Mario Music) (vs Dr. Mario)",
  "Great Bay (vs Young Link)",
  "Temple (vs Young Link)",
  "Corneria (vs Falco)",
  "Venom (vs Falco)",
  "Great Bay (Unused)",
  "PokÃ©mon Stadium (Pichu)",
  "Battlefield (Plays Mario Theme) (vs Team Mario & Bowser)",
  "Mushroom Kingdom II (vs Team Mario & Peach)",
  "Kongo Jungle (vs Team DK & Fox)",
  "Temple (vs Team Link & Zelda)",
  "Great Bay (vs Team Link & Young Link)",
  "Mushroom Kingdom I (vs Team Link & Luigi)",
  "Great Bay (Saria's Song) (vs Team Marth & Link)",
  "Big Blue (vs Team Samus & Captain Falcon)",
  "Brinstar (vs Team Samus & Fox)",
  "Yoshi's Story (vs Team Yoshi & Luigi)",
  "Yoshi's Island (vs Team Yoshi & Ness)",
  "Green Greens (vs Team Kirby & Pikachu)",
  "Fountain of Dreams (vs Team Kirby & Pichu)",
  "Green Greens (vs Team Kirby & Jigglypuff)",
  "Icicle Mountain (vs Team Kirby & Ice Climbers)",
  "Corneria (vs Team Fox & Falco)",
  "Mute City (vs Team Fox & Captain Falcon)",
  "PokÃ©mon Stadium (vs Team Pikachu & Pichu)",
  "PokÃ©mon Stadium (vs Team Pikachu & Jigglypuff)",
  "Mushroom Kingdom I (vs Team Luigi & Dr. Mario)",
  "Onett (alt music) (vs Team Ness & Peach)",
  "Fourside (vs Team Ness & Mewtwo)",
  "Big Blue (mRider song) (vs Team Captain Falcon & Falco)",
  "Battlefield (vs Team Bowser & Mewtwo)",
  "Battlefield (vs Team Bowser & Peach)",
  "Battlefield (vs Team Bowser & Zelda)",
  "Temple (vs Team Peach & Zelda)",
  "Great Bay (Saria's Song) (vs Team Zelda & Young Link)",
  "Temple (Emblem) (vs Team Zelda & Marth)",
  "Great Bay (Unused)",
  "Princess Peach's Castle (vs Giant Mario)",
  "Kongo Jungle (vs Giant DK)",
  "Great Bay (vs vs Giant Link)",
  "Yoshi's Story (vs Giant Yoshi)",
  "Mushroom Kingdom II (vs Giant Luigi)",
  "Mute City (vs Giant Captain Falcon)",
  "PokÃ©mon Stadium (vs Giant Jigglypuff)",
  "Fountain of Dreams (vs Giant Bowser)",
  "Mushroom Kingdom I (vs Giant Dr. Mario)",
  "Temple (vs Giant Young Link)",
  "Rainbow Cruise (vs Team Mario)",
  "Jungle Japes (vs Team Donkey Kong)",
  "Fountain of Dreams (vs Team Kirby)",
  "Mushroom Kingdom II (vs Team Luigi)",
  "Onett (vs Team Ness)",
  "PokÃ©mon Stadium (vs Team Jigglypuff)",
  "Icicle Mountain (Unused)",
  "PokÃ©mon Stadium (vs Team Pichu)",
  "Flat Zone (vs Team Game & Watch)",
  "Mute City (vs Team Captain Falcon)",
  "Battlefield (No items) (vs Metal Character)",
  "Final Destination (No items) (vs Master Hand)",
  "Rainbow Cruise (vs Mario)",
  "Kongo Jungle (vs Donkey Kong)",
  "Great Bay (vs Link)",
  "Brinstar (vs Samus)",
  "Yoshi's Story (vs Yoshi)",
  "Green Greens (vs Kirby)",
  "Corneria (vs Fox)",
  "PokÃ©mon Stadium (vs Pikachu)",
  "Mushroom Kingdom I (vs Luigi)",
  "Mute City (vs Captain Falcon)",
  "Onett (vs Ness)",
  "PokÃ© Floats (vs Jigglypuff)",
  "Icicle Mountain (vs Ice Climbers)",
  "Princess Peach's Castle (vs Peach)",
  "Temple (vs Zelda)",
  "Fountain of Dreams (Emblem Music) (vs Marth)",
  "Battlefield (PokÃ© Floats song) (vs Mewtwo)",
  "Yoshi's Island (vs Bowser)",
  "Mushroom Kingdom II (Dr Mario Music) (vs Dr Mario)",
  "Jungle Japes (vs Young Link)",
  "Venom (vs Falco)",
  "Fourside (vs Pichu)",
  "Final Destination (Emblem Music) (vs Roy)",
  "Flat Zone (vs Team Game & Watch)",
  "Brinstar Depths (vs Gannondorf)",
  "Battlefield (Event #01) (Trouble King)",
  "Temple (Event #18) (Link's Adventure)",
  "Princess Peach's Castle (Event #03) (Bomb-fest)",
  "Yoshi's Story (Event #04) (Dino-wrangling)",
  "Onett (Event #05) (Spare Change)",
  "Fountain of Dreams (Event #06) (Kirbys on Parade)",
  "PokÃ©mon Stadium (Event #07) (PokÃ©mon Battle)",
  "Brinstar (Event #08) (Hot Date on Brinstar)",
  "Great Bay (Event #09) (Hide 'n' Sheik)",
  "Yoshi's Island (Event #10) (All-Star Match 1-1 /vs Mario)",
  "Icicle Mountain (Event #11) (King of the Mountain)",
  "Mute City (Event #12) (Seconds, Anyone?)",
  "Rainbow Cruise  (Event #13) (Yoshi's Egg)",
  "Goomba  (Event #14) (Trophy Tussle 1)",
  "Battlefield (Event #44) (Mewtwo Strikes!)",
  "Corneria (Event #16) (Kirby's Air-raid)",
  "Jungle Japes (F-Zero Music) (Event #17) (Bounty Hunters)",
  "Kongo Jungle (Event #2) (Lord of the Jungle)",
  "Final Destination (Event #19) (Peach's Peril)",
  "Brinstar (Event #20) (All-Star Match 2-1 /vs Samus)",
  "Princess Peach's Castle (Event #21) (Ice Breaker)",
  "Mushroom Kingdom II (Event #22) (Super Mario 128)",
  "Brinstar Depths (Event #27) (Cold Armor)",
  "Yoshi's Island (Event #24) (The Yoshi Herd)",
  "Fourside (DK Rap) (Event #25) (Gargantuans)",
  "Entei (Event #26) (Trophy Tussle 2)",
  "Venom (Event #23) (Slippy's Invention)",
  "Green Greens (Event #28) (Puffballs Unite)",
  "Temple (Great Bay music) (Event #29) (Triforce Gathering)",
  "Fountain of Dreams (Event #15) (Girl Power)",
  "Mushroom Kingdom I (Event #31) (Mario Bros. Madness)",
  "Corneria (Many Arwings) (Event #32) (Target Acquired)",
  "F  //Zero Adventure Stage (Event #33) (Lethal Marathon)",
  "Great Bay (Event #34) (Seven Years)",
  "Dream Land (Event #35) (Time for a Checkup)",
  "Fourside (Event #36) (Space Travelers)",
  "Fountain of Dreams (Event #30) (All-Star Match 3-1 /vs Kirby)",
  "Mushroom Kingdom II (Event #38) (Super Mario Bros. 2)",
  "PokÃ©mon Stadium (Event #39) (Jigglypuff Live!)",
  "Temple (Emblem Music) (Event #40) (All-Star Match 4-1 /vs Marth)",
  "Temple (Emblem Music) (Event #41) (En Garde!)",
  "PokÃ© Floats (Event #42) (Trouble King 2)",
  "Big Blue (Event #43) (Birds of Prey)",
  "Battlefield (Event #37) (Legendary Pokemon)",
  "Flat Zone (Event #45) (Game and Watch Forever!)",
  "Temple (Emblem Music) (Event #46) (Fire Emblem Pride)",
  "Majora's Mask (Event #47) (Trophy Tussle 3)",
  "Yoshi's Story (Event #48) (Pikachu and Pichu)",
  "Mushroom Kingdom I  (Event #49) (All-Star Match Deluxe 5-1 /vs Dr Mario)",
  "Final Destination (Final Destination Match) (Event #50)",
  "Final Destination (The Showdown) (Event #51)",
  "Jungle Japes (DK Rap) (Event #10) (All-Star Match 1-2 /vs DK)",
  "Yoshi's Story (Event #10) (All-Star Match 1-3 /vs Yoshi)",
  "Princess Peach's Castle (Event #10) (All-Star Match 1-4 /vs Peach)",
  "Rainbow Cruise (Event #10) (All-Star Match 1-5 /vs Bowser)",
  "Great Bay  (All-Star Match 2-2 /vs Link)",
  "Temple  (All-Star Match 2-3 /vs Zelda)",
  "Mute City (All-Star Match 2-4 /vs Captain Falcon)",
  "Corneria (All-Star Match 2-5 /vs Fox)",
  "PokÃ©mon Stadium (All-Star Match 3-2 /vs Pikachu)",
  "Onett (All-Star Match 3-3 /vs Ness)",
  "Icicle Mountain (All-Star Match 3-4 /vs Ice Climbers)",
  "Mushroom Kingdom II (All-Star Match 4-2 /vs Luigi)",
  "PokÃ© Floats (All-Star Match 4-3 /vs Jigglypuff)",
  "Final Destination (All-Star Match 4-4 /vs Mewtwo)",
  "Flat Zone (All-Star Match 4-5 /vs Mr Game & Watch)",
  "Venom (All-Star Match Deluxe 5-2 /vs Falco)",
  "PokÃ©mon Stadium (All-Star Match Deluxe 5-3 /vs Pichu)",
  "Great Bay (Saria's Song) (All-Star Match Deluxe 5-4 /vs Young Link)",
  "Temple (Emblem Music) (All-Star Match Deluxe 5-5 /vs Roy)",
  "Final Destination (All-Star Match Deluxe 5-6 /vs Gannondorf)",
  "Battlefield (NO CHARA)",
  "PokÃ©mon Stadium //Unlocking Jigglypuff",
  "Final Destination (Great Bay music) //Unlocking Gannondorf?",
  "Mushroom Kingdom II //Unlocking Luigi",
  "Fountain of Dreams //Unlocking Marth",
  "Flat Zone //Unlocking Mr Game and Watch",
  "Princess Peach's Castle (DR Mario song) //Unlocking Dr Mario",
  "Final Destination //Unlocking Mewtwo?",
  "Great Bay (Saria's Song) //Unlocking Young Link",
  "Battlefield (Corneria Music) //Unlocking Falco",
  "PokÃ©mon Stadium //Unlocking Pichu?",
  "Temple (Emblem Music) //Unlocking Roy?",
  "Battlefield (Multi-Man Melee)"
};


