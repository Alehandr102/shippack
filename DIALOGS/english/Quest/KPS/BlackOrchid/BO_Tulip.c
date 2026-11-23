// Gregg: Clive's band and their chief, Tulip Banda - the "Black Orchid" trilogy
// (ported from Damned Destiny). Logic - Program\Quests\reaction_functions\blackorchid.c
// One file for four characters, as in the original:
//   1-5   - "MA Banda 1" (Clive) dressing down his men outside Port-au-Prince;
//   6-19  - Clive holds the player at gunpoint; either death or the road to Tulip;
//   20-43 - Tulip himself in the La Vega tavern, the bargain and the contract on Lanven;
//   44-57 - Clive lays out the plan of the assassination;
//   58    - Clive realises he has been betrayed and orders the attack.

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sName = GetHeroLastName();

	switch (Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "Clive: Right, we are here. Listen to me carefully, you fools. Through the town we go quietly, without haste. I do the talking with the soldiers, and you, Vincent, keep your mouth shut. Say one word without my order, like last time, and I put you down where you stand. Understood?";
			link.l1 = "Vincent: Understood, boss.";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Clive: Good. Now you, Jacky. Keep that piece behind your belt until I say to draw it. Do you understand me?";
			link.l1 = "Jacky: Boss, that weasel meant to slip away, I saw it myself. I fired - what else was I to do? He would have got clean off if I had not put one in him!";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "Clive: Jacky, it was a dead end! He was going nowhere; we would have cornered him and finished him quietly. Instead you started a gunfight, and half the garrison of Port Royale was on our heels.";
			link.l1 = "Jacky: But boss, you must admit I dropped him beautifully. Blew his head to rags, brains three yards across the street, I saw it myself. And Tulip was pleased.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Clive: So now you are proud of it?! Hand over the pistol! Now!";
			link.l1 = "Jacky: Boss, easy, I have got it. Piece behind the belt until you say. No trouble.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Clive: That is it, we move out!";
			link.l1 = "Vincent: Hey, boss, we have company.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactDialogWithBandaInPortPaxExit");
		break;

		case "6":
			dialog.text = "Clive: Look who has turned up. Jacky, get your piece out, you can practise your aim.";
			link.l1 = "Jacky: With pleasure, boss. Sleep well!";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Jacky, hold! Slow down. ...(peers into the face)... You are captain " + GetFullName(PChar) + ", are you not?";
			link.l1 = "I am.";
			link.l1.go = "8";
			link.l2 = "And what of it?";
			link.l2.go = "8";
			link.l3 = "No, you have taken me for someone else.";
			link.l3.go = "11";
		break;

		case "8":
			dialog.text = "Excellent, no need to go looking - another seeker right in front of us. Well, " + sName + ", this is the end of you. Would you like to die quickly and without pain? Then tell us everything you know about the Black Orchid.";
			link.l1 = "You are Clive, are you not? The same Clive who did for the governor of Maracaibo a couple of months ago?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Yes, I am Clive, one of the finest hired killers in the Caribbean. And you, " + sName + ", are in deep trouble. There is a contract on you. Do you follow? There is no wriggling out: Jacky has you covered, and if you run he will take your head off. Right, Jacky?";
			link.l1 = "Jacky: Just give the word, boss.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "There you are, " + sName + ", you heard Jacky. Tell us everything and you die quickly - you will not even feel it. Say nothing, and it will be the worse for you. Vincent is a master of the trade; he has a rich store of ways to make a man talk.";
			link.l1 = "...(to himself: I must talk my way out)... Now you listen to me, Clive. Listen carefully. I will speak only with your chief, with Tulip. Is that clear?";
			link.l1.go = "11";
			link.l2 = "...(to himself: I must talk my way out)... Listen, Clive, I have a business proposal for you.";
			link.l2.go = "12";
		break;

		case "11":
			dialog.text = "Clear enough! Jacky, get on with it.";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInPortPaxExitTownDead");
		break;

		case "12":
			dialog.text = "Money does not interest me, " + sName + ".";
			link.l1 = "I was not about to offer you money. My proposal is of another kind. I have long wanted to work for your chief, for Tulip.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Listen to him now. Not a bad try, a pity it failed. Tulip put out the contract, and I shall fulfil it. Nothing will stop me.";
			link.l1 = "Tulip will pay you far more if you bring me to him alive. I know a great deal that will interest him.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "Somehow I doubt that very much.";
			link.l1 = "Doubt what? That Tulip will pay more? Or that what I know will interest him?";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Both. Tulip never pays more than he promised, even if I deliver you alive. And there is nothing in this world capable of interesting him.";
			link.l1 = "Very well, then let us do it differently. If what I tell Tulip does not interest him and he orders my death, you kill me and take your money. If he lets me live, I pay you twice what he promised. Either way you lose nothing.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Twice what Tulip promised is five million.";
			link.l1 = "Five million? So Tulip values me at two and a half million piastres.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Just so, " + sName + ". Do you have that kind of money?";
			link.l1 = "The money will be there. That I promise. Well?";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Let me think ...(mutters something)... All right, " + sName + ", follow me and do not think of slipping away. Jacky, keep him covered.";
			link.l1 = "Where are we going? Is Tulip on Hispaniola?";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "That is right, he is here. Follow me and keep up.";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToLaVegaTavern");
		break;

		case "20":
			dialog.text = "Tell me what you know about the Black Orchid.";
			link.l1 = "What exactly shall I tell? I can tell you about the seven seekers of the Black Orchid.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "That I know already. What else?";
			link.l1 = "Would knowledge of the men who once possessed the Orchid interest you?";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "No, I know the legends of the six possessors perfectly well.";
			link.l1 = "There were seven, in fact. Name those you know, and I shall name the seventh.";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "The dock hand Giancarlo, the beggar Jack Strbeton, Alvarez Istilochos, Bertrand Grenadier, admiral don Pedro de la Encantario, Alejon Kendart.";
			link.l1 = "You are missing Stefan Lanven.";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "...(mockingly)... Stefan Lanven. Lanven never possessed the Orchid.";
			link.l1 = "But he saw it.";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "...(laughs)... Would you like me to tell you under what circumstances Lanven and I saw the Orchid?";
			link.l1 = "You and Lanven?!";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "Yes. I learned that the Black Orchid was with Bertrand Grenadier and set out to find him. Grenadier haunts brothels, so the whole search came down to one thing: looking for him in the brothels of the French colonies. Before long I found him on Tortuga. \nIt was night. I set an ambush to the left of the brothel door and waited. When Grenadier came out I took aim and was about to fire, when out of nowhere Lanven dropped on him.";
			link.l1 = "Dropped on him? How am I to understand that?";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "Just as I say. The brainless idiot leapt on Grenadier and tried to throttle him. Grenadier simply threw him to the ground and knocked him out with one blow of his fist. At that moment I fired... Damn it...";
			link.l1 = "What happened then?";
			link.l1.go = "28";
		break;

		case "28":
			dialog.text = "I missed. The ball went over his head and knocked off his hat. A second later Grenadier vanished, and in his place there remained a faint silver glow - the sign the Black Orchid leaves behind. Voices and the tramp of soldiers came near, and I ran for it. I got away; what became of Lanven I do not know. \nTwo months after that the Elusive Squadron arrived in the Caribbean. And, as you already know, the owner of the Black Orchid became admiral don Pedro de la Encantario. Damn it, had Lanven not been there I would have dropped Grenadier and taken the Orchid!";
			link.l1 = "Now I understand why Lanven never told me about this.";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "To business, " + sName + ". It is not for me to tell you about the Black Orchid, but for you to tell me. And enough chatter about seekers and possessors. Do you know anything of where the Orchid is now? Anything at all?";
			link.l1 = "I know that the Black Orchid is now in its rightful place.";
			link.l1.go = "30";
		break;

		case "30":
			dialog.text = "Its rightful place? And where might that be?";
			link.l1 = "Nobody knows. But I think it is where it was made. Where the Orchid was in the beginning. Is anything known of its origin?";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "No, nothing. The first to find it was the dock hand Giancarlo.";
			link.l1 = "Then perhaps that is the Orchid's rightful place? What if we were to look?";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "And how would you look, eh, " + sName + "? Nobody knows even the name of the colony, let alone the spot. And it would be foolish to hide such a treasure there. One man found it, another would too. No, old Kendart was no fool.";
			link.l1 = "And what of Kendart now? Where is he?";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "Find the Orchid and you will find Kendart. Or Kendart's corpse.";
			link.l1 = "Then perhaps we should look for Kendart rather than the Orchid? Find him and we find it.";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "Listen to you, talking as though we were partners already, searching for the Orchid together. Though I need only give the word and Clive will finish you. Do not forget that.";
			link.l1 = "But what I told you was useful, and besides...";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "...(with contempt)... Useful? ...(mimicking)... 'The Orchid is in its rightful place'. Nothing could be more useful. And how am I to know you are not lying? Perhaps you invented the whole thing to save your skin.";
			link.l1 = "It was Kendart himself who spoke of the rightful place.";
			link.l1.go = "36";
		break;

		case "36":
			dialog.text = "And there you have tripped yourself up, captain. Kendart himself told him. Do you even hear what you are saying?";
			link.l1 = "Not to me - to a friend of mine. A former friend. And he passed the words on to me.";
			link.l1.go = "37";
		break;

		case "37":
			dialog.text = "And who is this former friend?";
			link.l1 = "One of the seekers of the Orchid, Frederick Levinson.";
			link.l1.go = "38";
		break;

		case "38":
			dialog.text = "So you are acquainted with Levinson.";
			link.l1 = "I am, and have been for a long time.";
			link.l1.go = "39";
		break;

		case "39":
			dialog.text = "Excellent, then you know where he is now.";
			link.l1 = "Yes, I do. But...";
			link.l1.go = "40";
		break;

		case "40":
			dialog.text = "Splendid. Here is how it stands, captain: from now on you work for me, understood? Refuse and I kill you. Your first task is to kill Stefan Lanven. You have access to the governor's residence, do you not?";
			link.l1 = "I do.";
			link.l1.go = "41";
		break;

		case "41":
			dialog.text = "Excellent. It will cost you nothing to send that brainless idiot to the next world. Clive and his band will watch over your work and cover you if need be.";
			link.l1 = "I understand, Tulip.";
			link.l1.go = "42";
		break;

		case "42":
			dialog.text = "...(through his teeth)... But if you think of crossing me, captain, if Clive suspects you mean to slip away, if...";
			link.l1 = "I understand, I understand, I shall be a good boy.";
			link.l1.go = "43";
		break;

		case "43":
			dialog.text = "Splendid. Off with you. Clive and his lads will be with you.";
			link.l1 = "Very well. I am going.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInLaVegaTavernTalkWithClive");
		break;

		case "44":
			dialog.text = "Listen here, " + sName + ". From now on you work with me, with Clive, the best hired blade in the Caribbean.";
			link.l1 = "...(under his breath)... Small honour, working with a brute like you.";
			link.l1.go = "45";
		break;

		case "45":
			dialog.text = "What are you muttering there?";
			link.l1 = "I say it is a great honour to work with such a professional as yourself.";
			link.l1.go = "46";
		break;

		case "46":
			dialog.text = "That is more like it, " + sName + ". Now to business. I command this operation, understood?";
			link.l1 = "Understood.";
			link.l1.go = "47";
		break;

		case "47":
			dialog.text = "Here is how it goes, " + sName + ", listen well, I shall not repeat it. Now we go into town, to the governor's residence. You and I go inside and make for Lanven. You tell the soldier-boys that I am with you. Clear?";
			link.l1 = "Yes, clear.";
			link.l1.go = "48";
		break;

		case "48":
			dialog.text = "Then repeat what I said.";
			link.l1 = "You and I go into the residence and make for Lanven. I am to tell the soldiers that you are with me.";
			link.l1.go = "49";
		break;

		case "49":
			dialog.text = "Excellent, " + sName + ". We come to the residence, you present me, and then you keep Lanven talking. Understood?";
			link.l1 = "Not quite. How am I to present you?";
			link.l1.go = "50";
		break;

		case "50":
			dialog.text = "That is your affair, not mine. Say I am a friend of yours, or something of the sort. Invent something. Then get him talking about the Orchid - tell him, say, that you found traces of it in a cave on the Main.";
			link.l1 = "As if he would believe me.";
			link.l1.go = "51";
		break;

		case "51":
			dialog.text = "See that he does believe you, or I finish you along with Lanven.";
			link.l1 = "Fine, I shall manage it. And how do you mean to kill Lanven?";
			link.l1.go = "52";
		break;

		case "52":
			dialog.text = "Look here, " + sName + ", do you see these? ...(shows round, serrated metal plates)... They were invented in Japan, a deadly thing. Thrown properly, one will split a man's skull.";
			link.l1 = "I see. And what of the governor and the soldiers?";
			link.l1.go = "53";
		break;

		case "53":
			dialog.text = "What have the governor and the soldiers to do with it? We kill Lanven alone and slip out the back way. Jacky and Vincent will be waiting there to cover us if need be.";
			link.l1 = "The soldiers and the governor will be in the same room as Lanven. That residence has neither separate rooms nor a back way.";
			link.l1.go = "54";
		break;

		case "54":
			dialog.text = "You idiot, " + sName + "! Why could you not say so at once?";
			link.l1 = "You did not ask. I assumed you knew.";
			link.l1.go = "55";
		break;

		case "55":
			dialog.text = "We change the plan. Jacky and Vincent come with us. The moment we enter the residence they drop the soldiers and I finish Lanven. Your target, " + sName + ", is the governor. Do him with your pistol, never mind the noise.";
			link.l1 = "And after? How do we get away?";
			link.l1.go = "56";
		break;

		case "56":
			dialog.text = "As it comes, " + sName + ", as it comes, we shall see on the spot. That is it, we move out!";
			link.l1 = "...(under his breath)... The stupidest plan anyone could possibly devise.";
			link.l1.go = "57";
		break;

		case "57":
			dialog.text = "What are you muttering again?";
			link.l1 = "I say it is a magnificent plan, Clive, none better!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToPortPaxExitTown");
		break;

		case "58":
			dialog.text = "An ambush! The French were waiting for us! This is your doing, " + sName + "! Kill the captain!";
			link.l1 = "Damn it!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToPortPaxExitTownBattleWithTulipBandaBegin");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
