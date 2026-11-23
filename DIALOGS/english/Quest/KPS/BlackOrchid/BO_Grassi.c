// Gregg: Lukash de Grassi and his men - the "Black Orchid" trilogy (ported from
// Damned Destiny). Logic - Program\Quests\reaction_functions\blackorchid.c
// The player's companion from the second half of chapter 4:
//   First time - the stock line of de Grassi's sailors;
//   1-29  - the Bay of Doom: recruitment, refusal, bargain and the decision to storm Santiago;
//   30-34 - cape Camaguey, the scouting before the assault;
//   35-45 - the corsairs have upset the plan; a new one is born;
//   46-49 - Matthew refused, we go in without him;
//   50-51 - the town is taken, de Grassi launches his frigate;
//   52-58 - the trap: a squadron from the sea, guns in the jungle;
//   59-60 - the breakout.
// Merkes's squadron is three heavy frigates here, not ships of the line (see the block
// in MysteriousArtefactPreExitSantiagoEnd), so the text says frigates too.

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
			dialog.text = "Take all questions to Lukash.";
			link.l1 = "Very well.";
			link.l1.go = "exit";
		break;

		case "1":
			dialog.text = "And here comes captain " + sName + " - let us give him a welcome! Oh, I quite forgot to introduce myself. Lukash de Grassi, delighted to meet you.";
			link.l1 = "You do not look delighted, de Grassi. You work for Matilda, do you not?";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "I do, and must. And Matilda was right to expect you here. She knew Lanven's spies had got wind of our expedition and that he would send you.";
			link.l1 = "And your task is to kill me?";
			link.l1.go = "3";
		break;

		case "3":
			dialog.text = "...(breaks into a smile)... Captain, do you truly imagine we want you dead? You, one of the most successful seekers, who has come further in the search for the Orchid than all the rest put together? You are not only a source of valuable knowledge but a fine captain, able to carry out any task brilliantly. \nYou managed to find the key to Brian George's chest, while the fools working for Matilda hunted for it two months. When she was told the key was yours, she understood that you were exactly the man she had been lacking. Do you see what I am driving at?";
			link.l1 = "I do. But as it happens I work for Stefan Lanven, and I have not forgotten how Matilda nearly had me executed.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "The world is cruel, captain " + sName + ". Men use other men to reach their own ends. Only most often those 'other men' never suspect it. You, for instance, work for Lanven, though it is plain he uses you. He does not even pay you.";
			link.l1 = "Lanven has saved my life twice! Unlike Matilda, who nearly got me killed using me for her own ends. And you are gravely mistaken if you think Lanven does not pay for my work.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "...(thinks it over)... Hm... You are right, captain, I admit I was less than fair about Lanven. But believe me: he is using you. We know it.";
			link.l1 = "Then prove it!";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Gladly, captain. The proof was terribly hard to come by, but Lanven is not the only man with spies ...(smiles slyly)... That day when you gave Lanven the correct combination to the lock - do you remember it?";
			link.l1 = "Of course I remember.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "Lanven ordered you to wait four days. Supposedly that was the time he needed to read the book. But consider, captain: four days! There are little more than fifty pages in it. How long does it take to read? An ordinary man would need some five hours. And Lanven told you to wait four days. Why? I shall tell you. It was rumoured that the seeker Brian George had found the hiding place of the Orchid and set it down in his book. \nLanven reckoned that once he had the book he would find it at last. As soon as you handed it over he needed a pretext to be rid of you for a good while. You were no longer of use to him. So he told you to wait four days. The moment you left, Lanven's ship weighed anchor with him aboard and put out to sea. But he did not find what he wanted in the book and had to turn back. Six or seven hours after sailing from Martinique his ship was in port again.";
			link.l1 = "That does not sound especially convincing.";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "Believe it or not as you please. It no longer matters, because you do not work for Lanven any more.";
			link.l1 = "That is for me to decide!";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Hm... Did I not tell you? Lanven was found dead three days after you sailed from Martinique. And let me say at once: it was not our doing, not the Spaniards'. Because two of our own spies were killed along with him.";
			link.l1 = "...(with anger)... Of course, and I am to believe you, you scum. It was you who killed Lanven, your own work. And now you tell me he was using me, so that I should come over to Matilda.";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "But captain...";
			link.l1 = "Be quiet and listen, de Grassi. I refuse to work for Matilda. She wants me for the same thing Levinson wanted me, and Lanven, and Tulip: to run about doing errands. Is that clear? Do not try to force me, this is my final word.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Captain " + sName + ", we do not mean to force you. You are free to refuse, and I assure you we shall let you go. But think again. We offer an equal partnership. With Matilda your chances of finding the Orchid are better.";
			link.l1 = "A correction, de Grassi! Matilda's chances of finding the Orchid are better. And I, as always, get the honourable part of errand boy.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Very well, captain " + sName + ", I see you are not to be persuaded.";
			link.l1 = "Just so. Good day to you.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "...(pleading)... Captain, wait! Have mercy! My men and I are in a hopeless plight. I beg you, hear me out!";
			link.l1 = "...(taken aback)... All right, speak. I am listening.";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "A year ago I was a successful, promising captain of a beautiful frigate. I dreamed naively of one day becoming an admiral of Spain. But fate ruled otherwise. I was assigned, ship and crew and all, to that shrew Matilda. The year I worked for her was a true hell. She gave impossible tasks and paid coppers. In a year I lost more than three hundred men; my ship is falling apart, for the money barely covers provisions, powder and shot. \nThese last three months she has gone quite mad. Imagine - she ordered us to storm fort Marigot in a half-dead frigate with a crew of a hundred and seventy and a score of guns, half of which would not fire. By some miracle I broke the fort and crushed the garrison's resistance. And do you know what she paid us for it?";
			link.l1 = "Twenty thousand piastres?";
			link.l1.go = "15";
			link.l2 = "Twenty-five thousand?";
			link.l2.go = "15";
			link.l3 = "Thirty-five thousand?";
			link.l3.go = "15";
			link.l4 = "Fifty thousand?";
			link.l4.go = "15";
		break;

		case "15":
			dialog.text = "I should have been happy had she paid that much. But all I got was seven thousand piastres. Seven thousand!";
			link.l1 = "In your place I would have turned pirate rather than swallow such humiliation.";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "We resolved on it two months ago, but Matilda learned we were plotting something and forbade my ship to leave the port of Santiago. And a week ago I and the four survivors of my crew were brought here, to the Bay of Doom, aboard two frigates. Our task was simple: to persuade you to work for Matilda. She promised to pay us and release us from service. But it has proved far harder than that.";
			link.l1 = "So those frigates will come back for you before long, am I right?";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "No, captain. They will never come back for us. Matilda does not care what becomes of us. Our only chance is to persuade you to take us away.";
			link.l1 = "But this is the Main, you can reach civilisation through the jungle.";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "That is no jungle, it is a wall. There is no getting through. And even if we pushed a few miles in, the Indians would kill us. That is why the bay is called the Bay of Doom. There is no leaving it without a ship. So you were our only hope.";
			link.l1 = "Let me think. I agree to take you away. But in return you will help me find and kill Matilda. Agreed?";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Of course I agree, captain!";
			link.l1 = "Excellent! Then tell me everything you know, here and now.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Here and now? Captain, my men and I have not eaten for two days. And we are very tired. Perhaps you might feed us first and let us rest a little. I promise to tell you everything I know afterwards.";
			link.l1 = "No, de Grassi, tell it now. You will rest later.";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "As you wish, captain. I shall begin by saying that Matilda is an extraordinarily lucky seeker. Clive's band, on Tulip's orders, made several attempts to finish her, all in vain: she slipped away at the last moment every time. Once Clive and his men cornered her at the caves of Santo Domingo. \nUnderstanding that a fight with a pack of enraged pirates meant losing, she went down into the cave. Clive and his lads, eight or nine of them at the time, went after her. What happened below nobody knows, but Matilda got away and only three of Clive's band came out alive. She has always had the devil's own luck, captain.";
			link.l1 = "Come to the point, de Grassi. Where can she be found?";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "Her base is on Cuba, in Santiago. She is guarded no worse than the governor himself, and besides she has a bodyguard. His name is Verstehohlsten, a true cupboard of a man, near seven feet tall, with monstrous endurance. \nNo one has ever beaten Verstehohlsten. He wears his opponent down until the man can no longer hold a sword, then breaks his neck with his bare hands. So before you reach Matilda you will have to get past him.";
			link.l1 = "And this - what is his name - Verstehohlsten, is he always at her side?";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "Yes, captain. Of late Matilda does not leave the residence. And Verstehohlsten is always beside her - except, of course, the hours when she sleeps. Anticipating your question: getting into her bedchamber is impossible too.";
			link.l1 = "You know, de Grassi, an excellent thought has come to me. We shall storm Santiago!";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "Storm it? But why? Risk the ship, the crew, your own life, when it can all be done quietly?";
			link.l1 = "No, we storm from the land, and at a time when there are no ships in the port of Santiago. Once we break the fort's garrison and the townsfolk, if any stand, we take the residence. Matilda will not get away: no ships in port, and our lads will hold the gate. And killing Verstehohlsten is no great matter.";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "To my mind it would be better to work out a plan and do without an assault.";
			link.l1 = "...(with contempt)... A plan, a plan. Plans always fail. It will be as it was on Puerto Rico, when Levinson and I carried off the governor's daughter. Everything went to plan, we were within reach... but that is a long story. The point is, it was thought out to the last detail, and we still raised the alarm. So it is settled. We storm Santiago!";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "But captain " + sName + ", peaceful people will die, Spaniards, my own countrymen. Why kill hundreds of the innocent? And all to get at Matilda.";
			link.l1 = "You defend them only because they are your countrymen. Were it any other colony, French or Dutch, you would not have said a word about the townsfolk. Though you are right: many will die because of Matilda alone.";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "Perhaps we might work out a plan after all?";
			link.l1 = "Here is what we shall do, de Grassi. We shall write the governor of Santiago an ultimatum demanding that he give up Matilda. If he refuses, we storm the town.";
			link.l1.go = "28";
		break;

		case "28":
			dialog.text = "That is no way out. The governor is under orders to protect Matilda by every means. To him her life is worth more than the lives of every townsman in Santiago. He will not agree. And we shall lose surprise.";
			link.l1 = "Then the assault is unavoidable. That is my final decision and it is not open to discussion. Your task is to find out when there will be no ships in the harbour of Santiago. Otherwise Matilda will slip away. Can you manage it?";
			link.l1.go = "29";
		break;

		case "29":
			dialog.text = "I shall, captain.";
			link.l1 = "Then set course for Cuba, for cape Camaguey. We land there.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToSantiagoShore");
		break;

		case "30":
			dialog.text = "Captain, I have two pieces of news for you, one good and one bad. Which first?";
			link.l1 = "The good one, by tradition.";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "There is only one ship in the port of Santiago - the merchant fleut Philippe, and she sails tomorrow morning!";
			link.l1 = "Excellent. And the bad?";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "In three days a squadron of three heavy frigates under admiral Merkes arrives at Santiago. It will multiply the colony's defences several times over. Which means we have only two days for the assault.";
			link.l1 = "Two days will be enough. But how did you learn it so quickly? We have only just landed.";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "There were fishermen setting nets nearby, and I asked them. Have no fear, captain, they can be trusted - they hang about the tavern often and know no less than the tavern keepers themselves.";
			link.l1 = "Splendid. There is no time to lose; tomorrow morning we begin the assault on Santiago! And are you sure the fishermen will not let slip what you asked them?";
			link.l1.go = "34";
		break;

		case "34":
			dialog.text = "Quite sure, captain. They will never tell anybody anything again ...(smiles)...";
			link.l1 = "Hm... Perhaps that is for the best. Very well, de Grassi, tomorrow is a hard day; now we rest.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInSantiagoShoreTime");
		break;

		case "35":
			dialog.text = "Captain, we have bad news.";
			link.l1 = "What has happened?";
			link.l1.go = "36";
		break;

		case "36":
			dialog.text = "It is the corsairs. Last night a hundred of them stormed the fort of Santiago. They tried to break into the prison and free their chief Jason, but the Spaniards threw them back. Now they are gathering another party for a second attempt. Only nothing will come of it, damn them: the Spaniards have recovered and made ready. \nSeveral of the fort's guns are trained on the road, and the garrison has been strongly reinforced from Santiago. Matthew, the corsairs' leader, whom I spoke with this morning, asks our help. He promises to pay well if we help free Jason.";
			link.l1 = "Well - how much is well?";
			link.l1.go = "37";
		break;

		case "37":
			dialog.text = "He named no exact sum, but says it will cover all our losses and more. Only, captain, these cursed pirates have wrecked our plans: now the Spaniards expect an attack, and we have lost surprise.";
			link.l1 = "I think otherwise, de Grassi. Consider: the Spaniards expect an attack on the fort, not on the colony, and their main strength has been drawn there. We, with the corsairs behind us, strike the town, cut through to the residence and seize the governor and Matilda. And that is all. With hostages in hand we order the Spaniards in the fort to lay down their arms and give up Jason. And we lose no surprise at all: nobody expects an attack on the town.";
			link.l1.go = "38";
		break;

		case "38":
			dialog.text = "Captain, are you sure the Spaniards will lay down their arms rather than try to retake Santiago?";
			link.l1 = "But Matilda will be ours. You said yourself that keeping her alive matters to the Spaniards above all else.";
			link.l1.go = "39";
		break;

		case "39":
			dialog.text = "That is just the trouble, captain " + sName + ". The Spaniards are very mistrustful. They will not believe you would leave Matilda alive if they surrendered - the risk is too great. Most likely they will storm the town.";
			link.l1 = "Then we finish both Matilda and the governor. Those swine will regret starting the assault.";
			link.l1.go = "40";
		break;

		case "40":
			dialog.text = "But that will not help us, captain. The Spaniards will only be angrier, they will roll their guns up to the gates and close every road out. And in two days Merkes's squadron arrives and finishes us for good. If we are still alive by then.";
			link.l1 = "Damn! We must find a way out. Any ideas?";
			link.l1.go = "41";
		break;

		case "41":
			dialog.text = "There is! There is a way, captain! My frigate is in the docks under repair. If we can get her afloat before the Spaniards storm the town, we can leave on her!";
			link.l1 = "Your frigate? You said yourself she was an old wreck that...";
			link.l1.go = "42";
		break;

		case "42":
			dialog.text = "...(interrupting)... ...can still swim. At any rate she will get us to Camaguey. And there we go aboard your ship and make for Hispaniola.";
			link.l1 = "Are you certain? Will she really hold together?";
			link.l1.go = "43";
		break;

		case "43":
			dialog.text = "I cannot know for certain, captain. But if she will not reach Camaguey we shall land at Puerto Principe. And from there to the cape by land.";
			link.l1 = "Then it only remains to secure the pirates' support. Where do I find their leader?";
			link.l1.go = "44";
		break;

		case "44":
			dialog.text = "They are camped not far from Santiago, waiting on our answer. But, captain, if the Spaniards refuse to lay down their arms and storm the town, we shall not be able to free their chief Jason.";
			link.l1 = "And if they throw themselves at the fort, their chances of freeing Jason are none at all: the grapeshot will simply sweep them away. Attacking the town with us, they at least have a hope that the garrison will surrender. With us their odds are better.";
			link.l1.go = "45";
		break;

		case "45":
			dialog.text = "We need only persuade them to come with us. Their leader Matthew is stubborn; it will not be easy.";
			link.l1 = "Then I shall test the force of my persuasion. Let us go.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToSantiagoJungle");
		break;

		case "46":
			dialog.text = "Well, captain " + sName + ", did you persuade Matthew?";
			link.l1 = "...(angrily)... No, I did not. It is hopeless, the man is a stubborn ass. I should like to see how he storms that fort.";
			link.l1.go = "47";
		break;

		case "47":
			dialog.text = "Save your fury for the Spaniards. Let us discuss the plan of action, captain.";
			link.l1 = "What is there to discuss? We storm the town, break into the residence, kill the soldiers, take Matilda and the governor prisoner. What is unclear?";
			link.l1.go = "48";
		break;

		case "48":
			dialog.text = "And what then, captain?";
			link.l1 = "...(in a rage)... De Grassi, we have been over all of it, what more do you want?! The Spaniards in the fort surrender - excellent. They do not - we kill Matilda and the governor and leave on the frigate. What else?!";
			link.l1.go = "49";
		break;

		case "49":
			dialog.text = "No questions, captain.";
			link.l1 = "Then we move out!";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactPreBattleInSantiagoExitTown");
		break;

		case "50":
			dialog.text = "Fine work, captain! You fought splendidly!";
			link.l1 = "Remember the saying, de Grassi: do not cry hurrah before you have jumped. We have done half the work. So: take the men, go to the docks and get your frigate afloat. If there are holes, patch them quickly - we need only reach Puerto Principe. And I go to clear the residence. All clear?";
			link.l1.go = "51";
		break;

		case "51":
			dialog.text = "No questions, all clear.";
			link.l1 = "When I have finished at the residence I shall find you and we will settle what comes next. Go.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToSantiagoTownhall");
		break;

		case "52":
			dialog.text = "Things look bad, captain. It seems we are boxed in. Merkes's squadron is coming from the sea, and the way to the jungle is shut - Spaniards and guns.";
			link.l1 = "I know, de Grassi. Is your frigate afloat?";
			link.l1.go = "53";
		break;

		case "53":
			dialog.text = "She is, but... She will carry us to Puerto Principe, but fighting three of Merkes's ships... You understand, captain, it is a dead loss. The first broadside and we are food for fishes.";
			link.l1 = "Then we must break through to the jungle.";
			link.l1.go = "54";
		break;

		case "54":
			dialog.text = "Out of the question. The Spaniards have a dozen royal guns loaded with grapeshot, muskets, and the numbers. It is suicide.";
			link.l1 = "Damn! Are there other ways out of the town? Some underground passage, as in the castles of the old country.";
			link.l1.go = "55";
		break;

		case "55":
			dialog.text = "Even if there are, we have no time to look for them, captain. The squadron will be here in some four hours. What if we tried to dig in at the governor's residence? We have men enough to hold off an assault.";
			link.l1 = "Are you raving, de Grassi?! Fifteen hundred Spaniards will land in the town within five hours. And the Spaniards in the jungle - have you forgotten them? They will roll a couple of guns up to the residence and turn it into rubble.";
			link.l1.go = "56";
		break;

		case "56":
			dialog.text = "Yes, you are right, captain. But then we simply have no way out.";
			link.l1 = "Here is what we do, de Grassi: make the frigate ready for sea, we shall try to break through.";
			link.l1.go = "57";
		break;

		case "57":
			dialog.text = "In that wreck?!";
			link.l1 = "Have you any other proposal? If you have, I am listening.";
			link.l1.go = "58";
		break;

		case "58":
			dialog.text = "...(hanging his head)... No, captain.";
			link.l1 = "Just so. Gather every man and be ready for sea, and I shall find Matilda meanwhile - she is hiding somewhere in the town.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToSantiagoStore");
		break;

		case "59":
			dialog.text = "...(gloomily)... My frigate is still in the docks, captain. There is no leaving the bay - the fort's guns hold the throat of it, and Merkes is closing from the sea.";
			link.l1 = "Then we go by land. Our squadron is waiting at Camaguey - we make for it along the shore.";
			link.l1.go = "60";
		break;

		case "60":
			dialog.text = "...(more gloomily still)... Half a day's march, and that if the Spaniards do not come after us. And they will: Merkes is no fool, he will send ships along the coast to cut us off from the cape.";
			link.l1 = "Let him send them. What matters is reaching our own decks - after that we shall see who cuts off whom.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToSantiagoSea");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
