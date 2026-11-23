// Gregg: the Lus sisters and everything that follows them - "Two Sisters", main branch.
// One file for seven speakers: Rebekka, the onlooker at the door, Hanno Groot, Meril
// Kreno, Muhachi, Mishel and Terry Lorto.
// Logic - Program\Quests\reaction_functions\twosisters.c.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	switch (Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Anything else, captain?";
			link.l1 = "No.";
			link.l1.go = "exit";
		break;

		// The Tortuga brothel
		case "1":
			dialog.text = "Captain, would you care to earn a little?";
			link.l1 = "Depends on the work.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "My sister has been taken. Mishel Lus, she is known here. I pay whoever finds her.";
			link.l1 = "You are Rebekka Lus?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "I am. Do you know me?";
			link.l1 = "I was given two names, both Lus. I came for you both.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "...(warily)... For us? Speak plainly.";
			link.l1 = "A house is opening in San Juan. They need four women. You and your sister are two of them.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "We were offered that already and refused. Now I would agree to anything if it finds Mishel. Alone I go nowhere without her.";
			link.l1 = "Tell me everything, in order.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Three nights ago she left the house and never came back. A neighbour saw her being dragged towards the catacombs.";
			link.l1 = "Dragged by whom?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Two men in long robes. That is how she described them: no faces, and they walked in silence.";
			link.l1 = "And nobody followed?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Into the catacombs? ...(a joyless smile)... Nobody goes down there here, not even for money. Not everyone comes back, and not always as himself.";
			link.l1 = "I have been down there.";
			link.l1.go = "9";
			link.l2 = "What is down there?";
			link.l2.go = "10";
		break;

		case "9":
			dialog.text = "Then you know. And you still ask why nobody went after her?";
			link.l1 = "Fair. What is down there?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "Old vaults and bones. A passage to Fish Eye Bay, the smugglers use it. And so do those men in robes.";
			link.l1 = "Did anyone visit Mishel lately?";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "They did. The same sort, in robes. She talked with them for hours and never told me about what.";
			link.l1 = "Then it was her own people who took her.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "...(quietly)... Her own. Some own.";
			link.l1 = "I will go down into the catacombs.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "If you find her dead, tell me at once. Do not spare me, I will not survive a second wait.";
			link.l1 = "I will tell it as it is.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "Then go, captain. I will be here.";
			link.l1 = "...(leave)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestToDungeon");
		break;

		// After the catacombs
		case "15":
			dialog.text = "...(rises to meet you)... Well? Did you find her?";
			link.l1 = "I found this. By the way out to Fish Eye Bay.";
			link.l1.go = "16";
		break;

		case "16":
			TakeNItems(PChar, "TwoSisters_Medallion", -1);
			dialog.text = "...(takes the medallion)... Hers. She never took it off. So that is the way they led her.";
			link.l1 = "I found no body.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Then she is alive. ...(quickly)... Their ship left Fish Eye that same night, I asked. We need to learn whose it was.";
			link.l1 = "How will you learn that?";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "I will go round everyone who was on that shore. Nobody here refuses me, captain, I am one of theirs.";
			link.l1 = "How long do you need?";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Three days. Wait in the room above the tavern, I will come to you there.";
			link.l1 = "Three days I have.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Thank you, captain.";
			link.l1 = "Not yet, there is nothing to thank me for.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestToTavernRoom");
		break;

		// The tavern room, three days later
		case "24":
			dialog.text = "Captain! Quickly, come with me!";
			link.l1 = "Did you find out about the ship?";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "Not the ship. One of them is sitting in my house. I walked in and he was going through my chest.";
			link.l1 = "And you left him there alone?";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "I locked the door from outside. And called people, half the street is standing there now.";
			link.l1 = "Let us go.";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "The house is behind the tavern, up the street and to the left. A short flight of stairs and you run straight into the door. I will catch up.";
			link.l1 = "...(go)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusToTortugaTown");
		break;

		// The onlooker at the door
		case "28":
			dialog.text = "...(under his breath)... I would not go in there, captain. Two already did.";
			link.l1 = "And what became of them?";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "Nothing at all. In they went, and out they have not come. Ten minutes now.";
			link.l1 = "Step away from the door.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestToHouse");
		break;

		// The upstairs room: Hanno Groot over a body
		case "30":
			dialog.text = "...(breathing hard, leaning on the table)... Done for. Tough one, damn him.";
			link.l1 = "Who is done for?";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "The one in the robe. Olsen and I came in, he turned from the chest to face us. Olsen went down at once, and me he only caught.";
			link.l1 = "You killed him?";
			link.l1.go = "32";
		break;

		case "32":
			TakeNItems(PChar, "TwoSisters_AnimistMedallion", 1);
			dialog.text = "I did. ...(holds it out)... Took this off him. Some medallion, heavy. Might be of use to you, it will not be to me.";
			link.l1 = "Sit down and keep still, let me see the wound.";
			link.l1.go = "33";
		break;

		case "33":
			TwoSistersLusQuestToRoomSetAnimist();
			dialog.text = "...(looking past you)... Captain. Behind you.";
			link.l1 = "...(turn around)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestRoomFightWithAnimist");
		break;

		// Five days later, downstairs
		case "34":
			dialog.text = "...(seeing you)... On your feet. The surgeon said a week.";
			link.l1 = "How long was I down?";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "Three days. We found you upstairs, beside two dead captains.";
			link.l1 = "Two? And the third?";
			link.l1.go = "36";
		break;

		case "36":
			dialog.text = "There was no third. Smoke came pouring out of the doorway, sharp as a forge, and when it cleared there was only you and those two.";
			link.l1 = "Were you the only one who saw the smoke?";
			link.l1.go = "37";
		break;

		case "37":
			dialog.text = "The whole street saw it. And the soldiers who came running.";
			link.l1 = "There was a man in a robe in that room. He was lying on the floor with my blade in him.";
			link.l1.go = "38";
		break;

		case "38":
			dialog.text = "...(shakes her head)... There was nobody in a robe. Not alive, not dead.";
			link.l1 = "I took this medallion off him.";
			link.l1.go = "39";
		break;

		case "39":
			dialog.text = "The medallion was in your hand, that is true. The body was not there.";
			link.l1 = "Then he walked out himself.";
			link.l1.go = "40";
			link.l2 = "Then somebody carried him out.";
			link.l2.go = "40";
		break;

		case "40":
			dialog.text = "Or he played dead and left under the smoke. I would believe that, captain, but for one thing.";
			link.l1 = "What thing?";
			link.l1.go = "41";
		break;

		case "41":
			dialog.text = "Hanno Groot struck him twice. I heard it from below. After the second one, men do not get up.";
			link.l1 = "...(say nothing)...";
			link.l1.go = "42";
		break;

		case "42":
			dialog.text = "Put that thing away, captain. I cannot look at it.";
			link.l1 = "And the ship - you learned nothing?";
			link.l1.go = "43";
		break;

		case "43":
			dialog.text = "Nothing. Nobody saw anything that night, and those who did are keeping quiet.";
			link.l1 = "Then I go where they know about such medallions.";
			link.l1.go = "44";
		break;

		case "44":
			dialog.text = "You should be lying down another week.";
			link.l1 = "No time.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestNeedMedallionInfo");
		break;

		// Meril Kreno's house in Beliz
		case "56":
			dialog.text = "...(without turning)... At last, Casander! I have been waiting three days!";
			link.l1 = "I am not Casander.";
			link.l1.go = "58";
		break;

		case "58":
			dialog.text = "...(turns round)... Is that so. And what are you doing in my house?";
			link.l1 = "I was told you know who wears medallions like this one.";
			link.l1.go = "59";
		break;

		case "59":
			dialog.text = "Told by whom? Never mind. I have no time, I am sailing.";
			link.l1 = "I will pay for the conversation.";
			link.l1.go = "60";
		break;

		case "60":
			dialog.text = "I have money of my own. ...(waves a hand)... Go, captain, go.";
			link.l1 = "...(show the medallion)...";
			link.l1.go = "62";
		break;

		case "62":
			dialog.text = "...(falls silent and takes the medallion)...";
			link.l1 = "So do you know or not?";
			link.l1.go = "63";
		break;

		case "63":
			TwoSistersLusQuestSetCasanderToKrenoHouse();
			dialog.text = "...(slowly)... I know. But not here. I will tell you aboard, on the way.";
			link.l1 = "Aboard what?";
			link.l1.go = "64";
		break;

		case "64":
			SetCameraDialogMode(CharacterFromID("Captain Casander"));
			dialog.text = "Captain Casander: My apologies, senor Kreno. A squall held us up. We are ready to sail.";
			link.l1 = "...(turn round)...";
			link.l1.go = "65";
		break;

		case "65":
			SetCameraDialogMode(NPChar);
			dialog.text = "There he is. Casander, captain " + GetFullName(PChar) + " sails with us.";
			link.l1 = "I never said I was sailing with you.";
			link.l1.go = "66";
		break;

		case "66":
			locCameraFollow();
			dialog.text = "You will, if you want your answer. The ship is in Chetumal Bay, take the jungle road, it is shorter. I will catch up.";
			link.l1 = "I will be in the bay.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestToBelizShore");
		break;

		// Chetumal Bay
		case "67":
			dialog.text = "And there is captain " + GetFullName(PChar) + ". Come along, the boat is ready.";
			link.l1 = "...(approach)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestCasanderShotPrepare");
		break;

		// The hold of the Cerebus
		case "68":
			dialog.text = "Welcome aboard, captain " + GetFullName(PChar) + ". Your head aches, I imagine?";
			link.l1 = "You shot me in the back.";
			link.l1.go = "69";
		break;

		case "69":
			dialog.text = "Casander shot you. I merely did not stop him.";
			link.l1 = "What for?";
			link.l1.go = "70";
		break;

		case "70":
			dialog.text = "Not what for, but to what end. You will fetch a ransom. The owner of a colony is not cheap goods.";
			link.l1 = "You know about the medallions. Talk.";
			link.l1.go = "71";
		break;

		case "71":
			dialog.text = "...(sits down on a barrel)... Why not. There is nobody here to repeat it to.";
			link.l1 = "I am listening.";
			link.l1.go = "72";
		break;

		case "72":
			dialog.text = "Beliz is my town. I grew up in it and meant to stay its master. That took money and men, and men were found.";
			link.l1 = "Men in robes.";
			link.l1.go = "73";
		break;

		case "73":
			dialog.text = "Those very ones. They paid well and asked little: not to interfere. So I did not.";
			link.l1 = "Not to interfere with what, exactly?";
			link.l1.go = "74";
		break;

		case "74":
			dialog.text = "With what they do to the people they take away. I never asked. It was enough for me that they were taken from elsewhere.";
			link.l1 = "Mishel Lus was taken from Tortuga.";
			link.l1.go = "75";
		break;

		case "75":
			dialog.text = "And ended up on my list. Do you know what she did wrong? Nothing. She simply had a certain trinket, and the trinket was wanted.";
			link.l1 = "Is she alive?";
			link.l1.go = "76";
		break;

		case "76":
			dialog.text = "So far. I needed her healthy, not dead.";
			link.l1 = "And your conscience is quiet.";
			link.l1.go = "77";
		break;

		case "77":
			dialog.text = "Quite quiet. I do not carry one, as I do not carry spare ballast.";
			link.l1 = "Where is this ship going?";
			link.l1.go = "78";
		break;

		case "78":
			dialog.text = "That I will not tell you. ...(rises)... Rest, captain. It is a long way.";
			link.l1 = "You will come back.";
			link.l1.go = "79";
		break;

		case "79":
			dialog.text = "Of course I will. I still have to collect for you.";
			link.l1 = "...(watch him go)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestMainHeroStandUpPrepare");
		break;

		// Second anchorage, Kreno comes down again
		case "89":
			dialog.text = "We have reached the place, captain. Half a day at anchor, then on. ...(looks round the hold)... And you have not been idle, I see.";
			link.l1 = "There is nothing to do here.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestKrenoExitFromShip");
		break;

		// Muhachi
		case "90":
			dialog.text = "Stop, pale face. Name yourself.";
			link.l1 = "" + GetFullName(PChar) + ". I am not with them.";
			link.l1.go = "91";
		break;

		case "91":
			dialog.text = "Muhachi knows who you are with. We saw you come off their ship alone and with a blade in your hand.";
			link.l1 = "Who are you?";
			link.l1.go = "92";
		break;

		case "92":
			dialog.text = "I am Muhachi. This is my tribe. Our village stands beyond the river, and of the tribe's business I do not speak with pale faces.";
			link.l1 = "Then what do we speak of?";
			link.l1.go = "93";
		break;

		case "93":
			dialog.text = "Of those who came into the cave Kretu. It is ours, and now there are men in black robes inside it.";
			link.l1 = "Those are the ones I am after.";
			link.l1.go = "94";
		break;

		case "94":
			dialog.text = "Then our road is one road. Help drive them out, and Muhachi will not forget it.";
			link.l1 = "They hold a prisoner of mine. A woman.";
			link.l1.go = "95";
		break;

		case "95":
			dialog.text = "They have a woman, I have seen her. Living.";
			link.l1 = "I agree.";
			link.l1.go = "96";
		break;

		case "96":
			TakeNItems(PChar, "TwoSisters_MuhachiAmulet", 1);
			dialog.text = "Then take this. Wear it until the fighting ends.";
			link.l1 = "What is it?";
			link.l1.go = "97";
		break;

		case "97":
			dialog.text = "A charm. It makes their medallions empty.";
			link.l1 = "What do the medallions give them?";
			link.l1.go = "98";
		break;

		case "98":
			dialog.text = "Protection. It is hard to kill a man who wears one. The longer he wears it, the harder.";
			link.l1 = "I took one such medallion off a corpse. It gave me nothing.";
			link.l1.go = "99";
		break;

		case "99":
			TwoSistersLusQuestMessageIndsar();
			dialog.text = "Nor will it. You have worn it for days. The one you took it from wore it for years.";
			link.l1 = "So one could wear it and become untouchable?";
			link.l1.go = "100";
		break;

		case "100":
			dialog.text = "No. The medallion takes its payment. First weariness after a fight, then weariness without one, then the man does not rise at all.";
			link.l1 = "And they know this?";
			link.l1.go = "101";
		break;

		case "101":
			dialog.text = "They know. That is why they seek new ones when the old stop rising.";
			link.l1 = "...(understanding)... So that is what they need people for.";
			link.l1.go = "102";
		break;

		case "102":
			SetCameraDialogMode(NPChar);
			dialog.text = "Tribesman: Muhachi! They are coming!";
			link.l1 = "How many?";
			link.l1.go = "103";
		break;

		case "103":
			locCameraFollow();
			dialog.text = "Many. Keep close to my men, pale face.";
			link.l1 = "Do not touch the woman.";
			link.l1.go = "104";
		break;

		case "104":
			dialog.text = "My men will not touch her. For other hands I do not answer.";
			link.l1 = "...(make ready)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestFightInJungle");
		break;

		// After the fight
		case "109":
			dialog.text = "You fought well, pale face. Muhachi keeps his word.";
			link.l1 = "I need nothing.";
			link.l1.go = "110";
			link.l2 = "What did you promise?";
			link.l2.go = "110";
		break;

		case "110":
			dialog.text = "Need it or not, you will take it. We have a thing we do not know what to do with.";
			link.l1 = "What thing?";
			link.l1.go = "111";
		break;

		case "111":
			dialog.text = "A gun. Long, heavy, the barrel cut with grooves inside. Taken from a pale face who came too near the village.";
			link.l1 = "A rifled musket.";
			link.l1.go = "112";
		break;

		case "112":
			TakeNItems(PChar, "mushket_Shtuzer", 1);
			dialog.text = "Call it that. My men cannot handle it, and you will - if you know how. It is not for every hand.";
			link.l1 = "I know how.";
			link.l1.go = "113";
		break;

		case "113":
			dialog.text = "Then take it. And give back my charm, pale face.";
			link.l1 = "...(with regret)... May I not keep it?";
			link.l1.go = "114";
		break;

		case "114":
			TwoSistersLusQuestRemoveAllAmulets();
			dialog.text = "You may not. It is not yours, it belongs to the tribe.";
			link.l1 = "A pity.";
			link.l1.go = "115";
		break;

		case "115":
			TakeNItems(PChar, "MuhachiAmulet", 1);
			dialog.text = "...(a short laugh)... Take this one. Weaker, but it asks no payment. Your men will have an easier time under the guns.";
			link.l1 = "Thank you, Muhachi.";
			link.l1.go = "116";
		break;

		case "116":
			dialog.text = "And remember: do not go into the cave Kretu. Not alone, not with men.";
			link.l1 = "I was not going to.";
			link.l1.go = "117";
		break;

		case "117":
			dialog.text = "Had you been going to, I would not have said it. ...(raises a hand)... Go, the woman is waiting.";
			link.l1 = "Farewell.";
			link.l1.go = "118";
		break;

		case "118":
			dialog.text = "Not farewell. We will meet again, pale face, I know it.";
			link.l1 = "...(leave)...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestWaitInJungle");
		break;

		// Mishel Lus
		case "120":
			dialog.text = "...(backing away)... Who are you?";
			link.l1 = "You are safe. The rest I will tell you aboard, the ship is close.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestReturnToCerebus");
		break;

		case "121":
			dialog.text = "So who are you?";
			link.l1 = "Captain " + GetFullName(PChar) + ". Your sister sent me.";
			link.l1.go = "122";
		break;

		case "122":
			dialog.text = "Rebekka... ...(sits down on the ground)... So she was looking for me after all.";
			link.l1 = "Three weeks she looked. Why were you taken?";
			link.l1.go = "123";
		break;

		case "123":
			dialog.text = "For a trinket. A medallion, heavy thing, one of them left it with me against a debt.";
			link.l1 = "And you did not give it back.";
			link.l1.go = "124";
		break;

		case "124":
			dialog.text = "There is half a pound of gold in it and they offered me thirty pesos. Of course I did not.";
			link.l1 = "Over thirty pesos they nearly killed you.";
			link.l1.go = "125";
		break;

		case "125":
			dialog.text = "...(a wry smile)... And would you sell your ship for a couple of thousand because somebody asked politely?";
			link.l1 = "I would not.";
			link.l1.go = "126";
		break;

		case "126":
			dialog.text = "There you are. ...(more quietly)... Only I thought they would let it go. They came at night instead, four of them.";
			link.l1 = "Where were you being taken?";
			link.l1.go = "127";
		break;

		case "127":
			dialog.text = "To a cave, not far from here. They kept me there three days, and then you turned up, and those men with spears.";
			link.l1 = "Come. There is a ship waiting.";
			link.l1.go = "128";
		break;

		case "128":
			dialog.text = "Where to? Not Beliz, I beg you.";
			link.l1 = "Tortuga, to your sister. Beliz first: Casander's brig and the men left aboard her need a berth.";
			link.l1.go = "129";
		break;

		case "129":
			dialog.text = "...(sighs)... Very well. If it must be Beliz, then Beliz.";
			link.l1 = "Rest. It is not a long passage.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestReturnToBeliz");
		break;

		// The Cerebus cabin
		case "130":
			dialog.text = "...(seeing her sister)... Mishel...";
			link.l1 = "...(step aside)...";
			link.l1.go = "131";
		break;

		case "131":
			dialog.text = "Captain, I do not know how to pay you for this.";
			link.l1 = "You are both going to San Juan. That is payment enough.";
			link.l1.go = "132";
		break;

		case "132":
			dialog.text = "We are going. ...(firmly)... And today, before I remember to be afraid.";
			link.l1 = "Then get ready.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestComplete");
		break;

		// Terry Lorto in Beliz - the captured brig
		case "133":
			dialog.text = "Captain! ...(waves)... Terry Lorto, gunner. Casander had me on the oars for a fortnight.";
			link.l1 = "You brought his brig in?";
			link.l1.go = "134";
		break;

		case "134":
			dialog.text = "I did. Seven of us left aboard, all the same sort. We brought her to Beliz and anchored in the roads.";
			link.l1 = "Is she sound?";
			link.l1.go = "135";
		break;

		case "135":
			dialog.text = "Sound enough, only there is no crew for her. Seven men is not a crew, it is an embarrassment.";
			link.l1 = "Will you sail under me? The brig is yours, you will be her captain.";
			link.l1.go = "136";
			link.l2 = "Sell her here and split the money with the rest.";
			link.l2.go = "137";
		break;

		case "136":
			dialog.text = "...(straightens up)... I will, captain. And the seven with me. I shall find hands here in Beliz - there are hungry men enough in the roads, and they will come for a share.";
			link.l1 = "Then catch up.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestCerebusToSquadron");
		break;

		case "137":
			dialog.text = "As you say. A boat like that will fetch forty-five thousand, no more, she is knocked about. Here is your share.";
			link.l1 = "Take the rest and live on it.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("TwoSistersLusQuestCerebusToMoney");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
