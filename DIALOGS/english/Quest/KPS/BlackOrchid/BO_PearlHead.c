// Gregg: The headman of the pearl divers' village (HeadMan_1) - the "Black Orchid"
// trilogy, chapter 5 "The Gift of Destiny" (ported from Damned Destiny).
// Logic - Program\Quests\reaction_functions\blackorchid.c
// The village stands on the coast of the Main, by Amatike Bay; the fishery itself
// belongs to Santa Catalina. The player lies up here after the rout
// at Santiago and works off what the headman knows about Alejon Kendart:
//   First time - the dispatcher on PChar.DestinyGift (states 2-19);
//   1-17  - waking up, the bargaining and the agreement to work for the village;
//   18-25 - the first task: kill the two thieves;
//   26-31 - the talk about the missing pearls, the second task;
//   32-39 - the payment (850 piastres for 4 pearls) and the tale of Kendart;
//   40-46 - Feregueyro is back, the player is sent after Cristobal Maul;
//   47-52 - the village is saved, one last favour - kill Feregueyro;
//   53-60 - there is no reward, the third part of the Meritor instead, farewell.
// Nodes 1 and 47 are set by the logic (chr.Dialog.CurrentNode), every other entry
// goes through the First time dispatcher.
// Gregg: TempNode = "First time" is set BEFORE the switch. The original set it only
// at the very tail of the First time node, so after any branch that ended with
// AddDialogExitQuestFunction the dialog fell into an empty node.
// Gregg: the squadron at Santiago is three heavy frigates, not ships of the line
// (see BO_Grassi); node 3 has been brought in line with that.
// Gregg: place names taken from LocLables - Nikoya Bay (Shore50), Amatike Bay
// (Shore9), Karataska Lagoon (Shore10).

void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar, CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sBro = GetSexPhrase("brother", "sister");

	string sChief = NPChar.name;

	string sState = "";
	if (CheckAttribute(PChar, "DestinyGift")) sState = PChar.DestinyGift;


	switch (Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "What do you want?";
			link.l1 = "Nothing.";
			link.l1.go = "exit";

			if (sState == "2")
			{
				dialog.text = "Rest another day, " + sBro + ", then come to me.";
				link.l1 = "Very well.";
				link.l1.go = "exit";
			}

			if (sState == "3")
			{
				dialog.text = "Greetings, " + sBro + "! And you come just in time. Are you ready to work for the good of the village?";
				link.l1 = "I am here, so I am ready. What must I do?";
				link.l1.go = "18";
			}

			if (sState == "4")
			{
				dialog.text = "Have you killed those thieves, " + sBro + "?";
				link.l1 = "Er... Not yet.";
				link.l1.go = "exit";
			}

			if (sState == "5")
			{
				dialog.text = "Greetings, " + sBro + "! Is the work done?";
				link.l1 = "It is done, they are dead. I am waiting for my five hundred piastres. And for the tale of Kendart.";
				link.l1.go = "26";
			}

			if (sState == "6")
			{
				dialog.text = "Greetings, " + sBro + "! Have you brought the pearls?";
				link.l1 = "I am looking for them, " + sChief + ".";
				link.l1.go = "exit";
			}

			if (sState == "7")
			{
				dialog.text = "Greetings, " + sBro + "! Have you brought the pearls?";
				link.l1 = "I am looking for them, " + sChief + ".";
				link.l1.go = "exit";
			}

			if (sState == "9")
			{
				dialog.text = "Greetings, " + sBro + "! Have you brought the pearls?";
				link.l1 = "I have. Here are the four pearls, just as we agreed.";
				link.l1.go = "32";
			}

			if (sState == "10")
			{
				dialog.text = "Let me rest, " + sBro + ". Come back later.";
				link.l1 = "As you say...";
				link.l1.go = "exit";
			}

			if (sState == "11")
			{
				dialog.text = "Greetings, " + sBro + "! What brings you here?";
				link.l1 = "What else? Work, of course.";
				link.l1.go = "40";
			}

			if (sState == "12")
			{
				dialog.text = "Ah, " + sBro + "! Go and walk the bays for a while, look at the sea. There is no work for you.";
				link.l1 = "Very well, I shall.";
				link.l1.go = "exit";
			}

			if (sState == "13")
			{
				dialog.text = "Greetings, " + sBro + ". There is no work yet, come again later.";
				link.l1 = "That is not why I came. I have just returned from Nikoya Bay, and there...";
				link.l1.go = "41";
			}

			if (sState == "14")
			{
				dialog.text = "What the devil are you waiting for?!";
				link.l1 = "Do not shout at me. I am already on my way.";
				link.l1.go = "exit";
			}

			if (sState == "15")
			{
				dialog.text = "Bring Cristobal to the village! Every minute counts.";
				link.l1 = "We are coming. Not far now.";
				link.l1.go = "exit";
			}

			if (sState == "16")
			{
				dialog.text = "I hear there was fighting in the jungle. Do not tarry - they will be upon the village at any moment.";
				link.l1 = "I know. We are on our way.";
				link.l1.go = "exit";
			}

			if (sState == "17")
			{
				dialog.text = "Come and see me in the town hall. We shall talk there.";
				link.l1 = "I shall be there shortly.";
				link.l1.go = "exit";
			}

			if (sState == "18")
			{
				dialog.text = "Have you killed Feregueyro?";
				link.l1 = "I am going after him now.";
				link.l1.go = "exit";
			}

			if (sState == "19")
			{
				dialog.text = "Greetings, " + sBro + "! I can see it in your eyes - you have finished Feregueyro.";
				// Gregg: без «sloop» - класс корабля Ферегуэйро считается от флагмана игрока
				link.l1 = "I have. I ran him down, boarded him and finished Feregueyro myself.";
				link.l1.go = "53";
			}
			NextDiag.TempNode = "First time";
		break;

		case "1":
			dialog.text = "Oh! Awake at last. How do you feel?";
			link.l1 = "Alive, it seems. Only my left arm is burning like fire. Damn it, I remember nothing. Where am I?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "In the pearl divers' village, " + sBro + ". You were found on the shore of Amatike Bay - senseless, covered in cuts and bruises. We carried you to an empty house and patched you up as best we could - that is where you came round.";
			link.l1 = "And how long have I been here?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "A day and a little more. We had begun to think you would never come round. Tell me, " + sBro + ", do you remember anything at all? What happened to you?";
			link.l1 = "The last thing I remember is Santiago. Three heavy frigates had our ship boxed in. Then the deck went out from under me, I was in the water, and after that... after that, nothing.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Santiago, is it. No, impossible. The sea could not have carried you that long. From Santiago to Amatike is no short road, you would have died. Perhaps you remember something more?";
			link.l1 = "No, nothing. Beyond that fight my memory simply stops.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Then here is what I think, " + sBro + ". Most likely some trader passing Cuba picked you up. You lay senseless in his cabin for a few days, off Amatike pirates fell upon him, you went into the water again, and the waves washed you up to us. Well? Does that sound like the truth?";
			link.l1 = "It may be so. And did you find anybody besides me?";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "You were alone.";
			link.l1 = "And where are my things? And my money?";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Your things are in the chest, in that same house - every last piastre of it. Nobody laid a finger on it.";
			link.l1 = "Nobody? The divers who pulled me out - not one of them was tempted?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "I swear by the Black Orchid, " + sBro + " - not one of them! Not a single piastre taken!";
			link.l1 = "What did you say? You swear by the Black Orchid?";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "That is what I said. There was an old man living here who liked to swear by it. Whatever you asked him, it was always 'I swear by the Black Orchid'. So it stuck with me. And why have you started up so? Be easy, " + sBro + ", what you need now is rest.";
			link.l1 = "And this old man - was his name not Alejon Kendart?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "The very same. A curious old fellow, I will tell you. Every evening he span yarns about his past, and the whole village came to listen. And how do you come to know of him?";
			link.l1 = "Where is Kendart now? Where is he, answer me!";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Softly, " + sBro + ", softly. Kendart left us. He bought a launch a couple of months back and sailed off. Where, he did not say. And what do you want with the old man?";
			link.l1 = "You must tell me everything about Kendart. When he came, when he left, where he lived, what he did, whom he kept company with. Everything.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Mark what I am about to tell you, " + sBro + ": here nobody owes you anything. And nobody will do a thing for you for nothing. Not even I.";
			link.l1 = "I am asking only that you tell me about Kendart, nothing more. Is that so very much?";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "Must I say it again? I shall tell you nothing for nothing. You have money, I can see that well enough - and what good is it to me? There is nothing here to spend it on anyway. Work for me and for the good of the village in exchange for what I know. Work well and I shall tell you all of it.";
			link.l1 = "Devil take it, I have no time to work for you. I must get away from here and find Kendart.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "And how will you manage that? You have no ship, and there is none to be had here: the whole village owns nothing but skiffs and flat-bottoms, and not one of them goes beyond the shoals. Shake your purse all you like - nobody will carry you off this coast. So one road is left: work for me. There is no other.";
			link.l1 = "And if I pay? Pay handsomely - somebody will agree to take me.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "Keep your piastres, " + sBro + ". They will buy you neither a ship nor a fair wind here: there are no takers, and there will be none. Let us do as I said and all will be well: you learn of Kendart, and I settle the troubles of my village.";
			link.l1 = "Very well. And what am I to do? Dive for pearls with the rest?";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "No, that would be far too simple. The work will be of another sort. But that is for tomorrow; for now rest a day and get your strength back. Ah, and one thing more: the house you were laid up in was Kendart's, just across from mine. Stay on there. And bear us no grudge, " + sBro + ", that is how it has long been done here.";
			link.l1 = "I understand. And thank you for pulling me back from the other world.";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "Enough talk. Go and rest. Look in on me in three days or so - there will be work.";
			link.l1 = "I shall. And thank you again.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftRestTimer");
		break;

		case "18":
			dialog.text = "Send to the other world two scoundrels who will not let us live. No more than two months ago they were accused of stealing four large pearls. No solid proof was found, but they were stripped of the right to dive. Ever since they set upon the village and the sentries whenever the fancy takes them.";
			link.l1 = "And are you certain it was they who stole them? If the guilt was never proved, then they are innocent.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "There was no direct proof, but there was a cartload of the other kind, and all of it pointed at them. They were spared and let go, though by the law of this village they should have been shot. And now we reap the fruits of mercy, " + sBro + ".";
			link.l1 = "Then I have two questions. Where do I look for them, and what do I get for the work?";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Look for them? Anywhere at all. Give particular thought to the caves and grottoes. For the work I shall pay five hundred piastres.";
			link.l1 = "Five hundred piastres? That is a pittance!";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "Plus what I know of Kendart, if you have not forgotten. And five hundred piastres here is a great deal of money. Many a diver makes a hundred and fifty for a run.";
			link.l1 = "Now I see why those two turned thieves. On a wretched hundred and fifty I would have lifted five pearls myself.";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "And you would have paid for it with your head, " + sBro + ". The law is watched closely here. Nobody has yet carried a pearl away unnoticed.";
			link.l1 = "And why do men take such work at all? Labour fit for a galley, beggar's wages, no stealing, and a bullet at the first misstep.";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "For the beauty of it, " + sBro + "! For the sight of a pearl burning in your palm.";
			link.l1 = "Nothing simpler: buy a dozen pearls from any trader and gaze at them till your eyes ache.";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "No, " + sBro + ". That you will never understand.";
			link.l1 = "Of course not. How could a plain mortal like me?";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "Listen, " + sBro + ", go and work! You have worn me out with talk.";
			link.l1 = "Have the money ready, I shall be back soon.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftFirstTask");
		break;

		case "26":
			dialog.text = "Wait, do not rush me. Answer me this instead: did you search the bodies?";
			link.l1 = "Of course, I always do.";
			link.l1.go = "27";
			link.l2 = "No, I did not. Nothing was said about that in the task.";
			link.l2.go = "31";
		break;

		case "27":
			PChar.DestinyGift.FirstTalk = true;
			dialog.text = "And you found no four pearls upon them?";
			link.l1 = "No, they had no pearls at all. Should they have had?";
			link.l1.go = "28";
		break;

		case "28":
			dialog.text = "We have had proof that those two did indeed steal four large pearls.";
			link.l1 = "And now I am to find them?";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "Yes, " + sBro + ", that is your next task. Find them and I promise another three hundred piastres on top.";
			link.l1 = "I doubt they carried the pearls about with them - too great a risk of losing them. So they hid them somewhere. We shall look.";
			if (CheckAttribute(PChar, "DestinyGift.FirstTalk"))
			{
				if (PChar.DestinyGift.FirstTalk == true)
				{
					link.l1 = "If the pearls were not on them, then they hid them somewhere. We shall look.";
				}
			}
			link.l1.go = "30";
		break;

		case "30":
			dialog.text = "Search the grottoes and the caves, the cache is most likely there.";
			link.l1 = "Very well, I shall look.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftToSeachPearl");
		break;

		case "31":
			PChar.DestinyGift.FirstTalk = false;
			dialog.text = "Damn. That is very bad.";
			link.l1 = "Why?";
			link.l1.go = "29";
		break;

		case "32":
			AddMoneyToCharacter(PChar, 850);
			TakeNItems(PChar, "DestinyGiftPearl", -4);
			dialog.text = "Excellent. Here are eight hundred and fifty piastres. Honestly earned.";
			link.l1 = "And now tell me about Alejon Kendart.";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "Well then. Alejon Kendart turned up here some seven or eight months ago. An old man of middling height, dry as a stick, yet he carried himself as though the whole world came up to his knee. He came to me and said he hoped to find a quiet life in our village. At first I meant to turn him out, but he promised fifteen hundred piastres for the house. And there I could not hold out.";
			link.l1 = "Understandable enough, knowing what you earn here.";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "For the first fortnight Kendart never once left the house. And then he was as though another man: out of doors all day long, making acquaintance with everyone, talking with everyone. Within a month and a half he was the darling of the village - every evening he told stories out of his past.";
			link.l1 = "And did he never speak of the Black Orchid?";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "No, he only swore by it. We did ask what orchid that might be, and he held his tongue. And to tell the truth it was all the same to us. Then one day a fellow turned up here, Murdok his name was, I think.";
			link.l1 = "Murdoch? Sir Andrew Murdoch?";
			link.l1.go = "36";
		break;

		case "36":
			dialog.text = "Yes, that is right, Andrew Murdoch. He began questioning me about Alejon Kendart, and he knew for certain that Kendart lived here. At first I refused, and then he offered me money, and I...";
			link.l1 = "I see, you agreed. What then?";
			link.l1.go = "37";
		break;

		case "37":
			dialog.text = "I told him where Kendart lived, and then...";
			link.l1 = "And then what?";
			link.l1.go = "38";
		break;

		case "38":
			dialog.text = "That is the end of the story, " + sBro + ". Do a couple more errands for me and I shall tell you what came after.";
			link.l1 = "Damn you! Very well, you have won. What errands?";
			link.l1.go = "39";
		break;

		case "39":
			dialog.text = "Come in two or three days and there will be work for you.";
			link.l1 = "Very well, I shall come.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftNextTimer");
		break;

		case "40":
			dialog.text = "I have no work for you yet. Do not take it ill, " + sBro + ", that is how it has fallen out. And since you have nothing to do, walk about the country round the village, look at the sea from the bay, climb about the caves.";
			link.l1 = "Very well, I shall take a walk.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftNoneTask");
		break;

		case "41":
			dialog.text = "...(cutting in)... I know, I know, there is a fine view of the sea there.";
			link.l1 = "No, there is a man in a robe there! A very strange one.";
			link.l1.go = "42";
		break;

		case "42":
			dialog.text = "...(turns pale)... A man in a robe? What did he say?";
			link.l1 = "That all will be killed and none will survive. That Feregueyro will be avenged and the curse lifted.";
			link.l1.go = "43";
		break;

		case "43":
			dialog.text = "...(turns paler still)... Feregueyro has come back. No! Not that! Not now!";
			link.l1 = "Hey, " + sChief + "! " + sChief + ", stop shouting. Be calm!";
			link.l1.go = "44";
		break;

		case "44":
			dialog.text = "Ruin, " + sBro + "! Ruin! Feregueyro has come back to our shores.";
			link.l1 = "And who is this Feregueyro? Why do you fear him so? Explain, at last!";
			link.l1.go = "45";
		break;

		case "45":
			dialog.text = "There is no time to explain, " + sBro + ". The village must be roused and made ready to defend itself - a fight with Feregueyro's servants cannot be avoided. So here is your next task: find a man named Cristobal Maul and bring him here, to the village. In a brawl he is beyond price.";
			link.l1 = "Where am I to look for him?";
			link.l1.go = "46";
		break;

		case "46":
			dialog.text = "I do not know, " + sBro + ". Somewhere in the jungle, near the caves perhaps. The thing is to find him and bring him here. And make haste, there is no time at all: Feregueyro's servants will be upon us at any hour.";
			link.l1 = "I am on my way!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftSearchKuvalda");
		break;

		case "47":
			dialog.text = "The devil, where have you been, " + sBro + "? Why did it take you so long to find Cristobal?";
			link.l1 = "We had to fight Feregueyro's servants twice before we reached the village! And why were you sitting in your house instead of fighting in the street with the rest?";
			link.l1.go = "48";
		break;

		case "48":
			dialog.text = "It is my duty to stay by the pearls at all times! I am the last line of defence of our wealth.";
			link.l1 = "I see. Strange customs you keep here. Well then: the village is saved, the attack is beaten off. I take it that is worth letting me go at last.";
			link.l1.go = "49";
		break;

		case "49":
			dialog.text = "We are in your debt, " + sBro + "! But first do the village one last service.";
			link.l1 = "As it happens, I am rather tired.";
			link.l1.go = "50";
		break;

		case "50":
			dialog.text = "You will save a great many lives if you agree.";
			link.l1 = "Very well. What must I do?";
			link.l1.go = "51";
		break;

		case "51":
			// Gregg: в оригинале тут дарили люггер «Броненосец». У нас возвращается свой
			// корабль - см. BO_RestockFlagship в blackorchid.c. Класс корабля Ферегуэйро
			// тоже не фиксирован, поэтому «шлюп» заменён на нейтральное.
			dialog.text = "Kill Feregueyro! We have learned that he is making ready to put out from Karataska Lagoon. Run him down and kill him. Avenge our people, our village, all the evil he has done us. We shall not leave you unrewarded, " + sBro + ", I promise. And one thing more. While you were in the jungle, two men came here - they named themselves your crew and were looking for you. They did not find you and went back to Amatike Bay. The ship is patched up and stocked, they say. They are waiting for you.";
			link.l1 = "Very well, I shall help you.";
			link.l1.go = "52";
		break;

		case "52":
			dialog.text = "And one thing more. Do not sink him, " + sBro + ". Take him by boarding and bring him up on deck alive. Those of ours who saw him close say he wears some trinket on a cord and does not take it off even in a fight. A charm, no doubt. If he drowns, the trinket goes with him - and I should like to see what charm kept such a man safe.";
			link.l1 = "Mm...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftNeedKillFeregueyro");
		break;

		case "53":
			dialog.text = "Did he beg for mercy? Did he plead with you?";
			link.l1 = "No, he met his death well. I think it is time we spoke of the reward.";
			link.l1.go = "54";
		break;

		case "54":
			dialog.text = "Mmm... You know, " + sBro + ", there will be no reward.";
			link.l1 = "I do not follow. What do you mean, no reward?";
			link.l1.go = "55";
		break;

		case "55":
			dialog.text = "After the last attack all our savings, every pearl, vanished. All we have left is this thing here - we found it in the house where Kendart lived. Perhaps you will take it as your reward? You were asking after Kendart and everything to do with him.";
			link.l1 = "Devil take me, that is the third part of the Meritor!";
			link.l1.go = "56";
		break;

		case "56":
			dialog.text = "The third part of what?";
			link.l1 = "Never mind. I shall take it.";
			link.l1.go = "57";
		break;

		case "57":
			TakeNItems(PChar, "MysteriousArtefact_MeritorChapter_3", 1);
			dialog.text = "Good. Take it, and I hope you get some use out of it.";
			link.l1 = "Have no doubt of that. Now I must go.";
			link.l1.go = "58";
		break;

		case "58":
			dialog.text = "Wait, " + sBro + ". There was one more thing I wished to say to you.";
			link.l1 = "Be quick about it.";
			link.l1.go = "59";
		break;

		case "59":
			dialog.text = "Thank you for everything! The people of our village are grateful for what you have done for them. We shall never forget it. Thank you!";
			link.l1 = "...(abashed)... I did nothing out of the ordinary.";
			link.l1.go = "60";
		break;

		case "60":
			dialog.text = "Farewell, " + sBro + ". And good luck to you.";
			link.l1 = "And good luck to you. And to everyone in the village. Farewell.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("DestinyGiftExitPearlVillage");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
