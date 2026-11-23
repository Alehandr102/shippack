// Gregg: the French soldiers and the commandant of Port-au-Prince - the "Black Orchid"
// trilogy (ported from Damned Destiny). Logic - Program\Quests\reaction_functions\blackorchid.c
// One file for three characters, as in the original:
//   1-2   - the street patrol, arrest for the murder of Bertrand Grenadier;
//   3-28  - the interrogation in the prison and the terms of release;
//   29-33 - the sentry at Brian George's house, demanding a written permit;
//   34-46, 50 - the commandant after the ambush on Tulip's band, hands over the permit;
//   51-54 - the sentry accepts the permit and unlocks the house.
// Nodes 47-49 of the original (the side quest about the beast near the fort) were not
// ported - that is a separate Damned Destiny quest we do not have.

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
			dialog.text = "Captain " + GetFullName(PChar) + "! You are under arrest for the murder of Bertrand Grenadier! Anything you say may be used against you. Come along!";
			link.l1 = "The whole thing was a set-up! I am innocent!";
			link.l1.go = "2";
		break;

		case "2":
			dialog.text = "Come along! They will sort it out at the prison.";
			link.l1 = "Damn it.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToPortPaxPrison");
		break;

		case "3":
			dialog.text = "Captain " + GetFullName(PChar) + "! The interrogation will now be conducted. Although we know for certain that you killed captain Bertrand Grenadier, I should like to hear the motives that drove you to this crime. Speak.";
			link.l1 = "As I was leaving the tavern, a weeping woman ran up to me and begged for help. She and her husband had been resting on the glade beyond the town gates when a drunken captain came up and began to insult her. Her husband defended his wife's honour and was shot dead with a pistol, after which that captain violated the woman. I went out of town, to the place where it happened. On that glade this same Bertrand Grenadier attacked me. Defending my own life, I was forced to kill him.";
			link.l1.go = "4";
		break;

		case "4":
			dialog.text = "Bertrand Grenadier attacked you without warning?";
			link.l1 = "Yes, commandant. I barely had time to draw my sword and parry his blow.";
			link.l1.go = "5";
		break;

		case "5":
			dialog.text = "Captain " + sName + ", did the woman who asked for your help give her name?";
			link.l1 = "No, commandant, she did not.";
			link.l1.go = "6";
		break;

		case "6":
			dialog.text = "Describe her appearance. It matters to us how she looked.";
			link.l1 = "Short of stature, in a red gown with blue sleeves.";
			link.l1.go = "7";
		break;

		case "7":
			dialog.text = "That will do. Yes, it all fits. The interrogation is over, captain " + sName + ". And now listen carefully. We are prepared to release you from this prison, but in return you will do France a service. Agreed?";
			link.l1 = "And what service would that be?";
			link.l1.go = "8";
		break;

		case "8":
			dialog.text = "You are to find and kill a person undesirable to the government of France. You have no choice - this work is your only chance to get out of here. Agreed?";
			link.l1 = "My only chance to get out of here? I would wager it is not the only one...";
			link.l1.go = "9";
		break;

		case "9":
			dialog.text = "Captain " + sName + ", you killed Bertrand Grenadier, a hero of France! The governor has ordered you executed on the town square at once, without interrogation or inquiry. The enraged crowd is thirsting for the blood of the man who killed their favourite, and only the timely intervention of Stefan Lanven, who talked me into holding this interrogation and offering you your life and freedom, keeps me from giving the order to the escort.";
			link.l1 = "Stefan Lanven? Who is that?";
			link.l1.go = "10";
		break;

		case "10":
			dialog.text = "YES or NO, captain " + sName + "?! The clock is running in seconds!";
			link.l1 = "Very well, I agree! I promise to carry out the task.";
			link.l1.go = "11";
		break;

		case "11":
			dialog.text = "Had you a little more sense, you would have agreed at once. And now, captain, prepare to listen. I shall tell you about Bertrand Grenadier so that you finally understand whom you killed. Do not interrupt; all questions after my account. Is that clear?";
			link.l1 = "Yes, commandant.";
			link.l1.go = "12";
		break;

		case "12":
			dialog.text = "Captain Bertrand Grenadier is a hero of France. Some years ago he prevented an attack on the French colony of Port-au-Prince by killing Alvarez Istilochos, a Spanish captain and the deadliest enemy of France. A record bounty was set on Istilochos's head - 299,999 piastres. In Spain Alvarez was a national hero, wildly popular, and he carried out every commission of governor-general Francisco Oregon y Goscon brilliantly. \nTo defeat Istilochos, Grenadier had to become a double agent. Remaining a loyal subject of the French Crown, he entered the service of Spain. Succeeding at every task set him, Grenadier soon won fame, while Istilochos fell from favour after botching an important commission. Alvarez, swiftly losing the reputation he had laboured for, was seized with fury and hatred for Grenadier. \nFor it was Grenadier who had become the hero in his place, Grenadier who stood at the peak of popularity. Alvarez was not used to second place, nor to sharing glory. He devised a cunning plan: to regain what he had lost and be rid of his rival at the same time. \nAlvarez proposed to Grenadier that they attack Port-au-Prince, whose fort was reckoned among the strongest in the Caribbean. Istilochos meant to shell the fort and, during the assault, to kill Grenadier unseen with a pistol shot. But Bertrand was no fool either. By night Istilochos's manowar, the Francis, and Grenadier's frigate, the Esthete, came up to the lighthouse of Port-au-Prince. \nA council of war gathered in the cabin of the Francis, with officers from every ship attending. At that same hour, according to Grenadier's carefully laid plan, a fireship was being built aboard the Esthete. It was put together in a matter of minutes. Under cover of darkness the fireship, handled by a handful of bold sailors, bumped safely against the manowar's side. \nHaving made the fireship fast, the sailors set it alight and left the vessel. The flames leapt instantly to the side of the Francis, and a few seconds later the powder barrels went up. The explosion was the signal for Grenadier and his officers. Drawing their swords, they fell upon Istilochos's men. \nThe surprise told: the stunned enemy could offer no resistance. As soon as Alvarez and his men were down, Grenadier and his officers leapt through the stern windows and swam for the Esthete. By the time Bertrand reached the side of his own ship, the Francis was burning brightly in the pre-dawn haze. Such was the end of Alvarez Istilochos. Any questions?";
			link.l1 = "Yes, commandant. How could you be sure that Grenadier would not one fine day betray France and become a loyal subject of the Spanish Crown? You could not control him while he served Spain. Double agents are an enormous risk; one can never be entirely certain whose side they are on.";
			link.l1.go = "13";
		break;

		case "13":
			dialog.text = "A good question, captain. It was all in his father, Pierre. Before the eyes of thirteen-year-old Bertrand, Pierre Grenadier was killed by Spanish soldiers during the storming of Martinique. Bertrand swore to avenge himself on the Spaniards all his life. Any more questions?";
			link.l1 = "How did Bertrand learn that Istilochos meant to kill him during the assault?";
			link.l1.go = "14";
		break;

		case "14":
			dialog.text = "It was plain from the start. Istilochos always sailed alone, without companions. There is a tale of a certain captain who sailed with him when Alvarez was still unknown. Together they sacked a French settlement, and Istilochos did all the work. \nYet the honour and the glory went not to him but to his companion. Enraged, Alvarez refused to sail with the man and took his ship out of the squadron. Later his former companion was found dead in the cabin of his own ship. But that is only a legend, no more. \nSo the very fact that Istilochos offered Grenadier a partnership was suggestive. And Bertrand's sixth sense told him he would be killed: Alvarez was not one to share glory. Questions?";
			link.l1 = "How could the manowar's crew fail to notice an approaching fireship? Where were the lookouts? And the bustle aboard the Esthete while the fireship was built is hard to miss as well.";
			link.l1.go = "15";
		break;

		case "15":
			dialog.text = "The crew of the Francis could not have seen the fireship built, for two reasons. First, it was assembled on the starboard side, and only the port side was visible from the manowar. Second, the night was very dark; nothing could be made out at five paces. \nFor the same reason they did not see the fireship come on. And by the time they did see it and understood, it was too late: the fireship was burning, and only seconds remained before the powder went up. That is how it was. Any more questions?";
			link.l1 = "Did anyone survive from the Francis?";
			link.l1.go = "16";
		break;

		case "16":
			dialog.text = "Several dozen men survived. All of them were hanged.";
			link.l1 = "All of them?!";
			link.l1.go = "17";
		break;

		case "17":
			dialog.text = "All who were taken. We French hated Istilochos, his ship, and everyone who sailed with him. We hanged them to sate our revenge in full. Enough questions, captain " + sName + ", let us come to business. First, your task. The woman who asked for your help is a Spanish agent. It is she you are to kill.";
			link.l1 = "What?! She is a Spanish agent?";
			link.l1.go = "18";
		break;

		case "18":
			dialog.text = "Do not interrupt! She is a Spanish agent, and her object was the death of Bertrand Grenadier. Through you she achieved it.";
			link.l1 = "She deceived me! And Grenadier attacked me first.";
			link.l1.go = "19";
		break;

		case "19":
			dialog.text = "Do not interrupt me, captain " + sName + "! We know the circumstances of his death. We know the woman deceived you. We know Grenadier struck first. \nWe know all of it - but do not forget that you are Bertrand's killer, and most of my countrymen would tear you to pieces even if you tried to justify yourself. And for the last time, I ask you not to interrupt. Is that clear?";
			link.l1 = "Yes, commandant. One question, if you permit.";
			link.l1.go = "20";
		break;

		case "20":
			dialog.text = "Ask it.";
			link.l1 = "At the start of the interrogation you let fall the words: 'It all fits'. What did that mean?";
			link.l1.go = "21";
		break;

		case "21":
			dialog.text = "A woman in a blue gown ran up to one of the soldiers keeping order in the streets and told him that a certain captain " + sName + " - that is, you - intended to kill Bertrand Grenadier. When the soldier reached the gates, he saw you with a bloodied sword and placed you under arrest.";
			link.l1 = "But how did you learn she was a Spanish agent? Damn! Forgive me, I interrupted.";
			link.l1.go = "22";
		break;

		case "22":
			dialog.text = "We all call that woman Matilda - nobody knows her real name. She has a dozen operations of this kind to her credit. She finds her victim in town and says her husband has been killed and she herself violated. Then she begs him to punish the murderer, who is about to appear beyond the gates. \nThe captain yields to her tearful pleas and waits outside. Then Matilda finds another captain and repeats the same tale with slight changes. But why do I tell you this - you remember perfectly well what she said to you. From there it goes by the script you already know: one man dies beyond the gates, the other is arrested by the soldiers on her word.";
			link.l1 = "What a cunning witch!";
			link.l1.go = "23";
		break;

		case "23":
			dialog.text = "Devilishly well put, captain. Well, I have already told you more than Stefan Lanven required of me. You will receive further instructions from him; he will be waiting for you in the governor's residence on Martinique. Is that all clear?";
			link.l1 = "Yes, commandant. I am ready.";
			link.l1.go = "24";
		break;

		case "24":
			dialog.text = "A piece of advice free of charge, captain " + sName + ". Stefan Lanven is a hard man. You would do well to obey him without argument. I know that you free captains love your liberty and are not used to being commanded, but you owe Lanven your life. Remember it, even if Stefan is rude to you or demands the impossible.";
			link.l1 = "Am I to hold my tongue even if he sends me to certain death?";
			link.l1.go = "25";
		break;

		case "25":
			dialog.text = "Many captains have worked under Lanven, and not one of them died. And in any case, plenty of men dream of working for him. Count yourself lucky. And I remind you again: you owe Lanven your life.";
			link.l1 = "Yes, I understand, I understand. Now let me out.";
			link.l1.go = "26";
		break;

		case "26":
			dialog.text = "Captain, did I not warn you?";
			link.l1 = "About what?";
			link.l1.go = "27";
		break;

		case "27":
			dialog.text = "You are to spend three days in this prison before you are set free.";
			link.l1 = "Devil take it, you promised to release me!";
			link.l1.go = "28";
		break;

		case "28":
			dialog.text = "So I did. But not a day sooner than three. That is our condition. I hope our gruel is to your taste. Good day to you.";
			link.l1 = "...";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactInPrisonTime");
		break;

		case "29":
			dialog.text = "What do you want?";
			link.l1 = "I need to get into this house. I act on the orders of Stefan Lanven.";
			link.l1.go = "30";
		break;

		case "30":
			dialog.text = "Forbidden! The commandant ordered that nobody be let in. If you want to pass, go to the commandant, get a permit, show it to me, and I shall let you through.";
			link.l1 = "And what happened here, anyway? Why is nobody allowed in?";
			link.l1.go = "31";
		break;

		case "31":
			dialog.text = "There has been a murder here. An investigation is under way.";
			link.l1 = "Who was killed? Jacques the drunkard?";
			link.l1.go = "32";
		break;

		case "32":
			dialog.text = "And what business is that of yours?";
			link.l1 = "I work for Stefan Lanven, and I must know what happened here.";
			link.l1.go = "33";
		break;

		case "33":
			dialog.text = "Off you go to the commandant for a permit. Without one you do not pass. That is all.";
			link.l1 = "Argh! Fine, you shall have your permit.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToPortPaxJailOff");
		break;

		case "34":
			dialog.text = "Good day, captain " + sName + ". Alive, unwounded?";
			link.l1 = "I am quite all right. Not a scratch.";
			link.l1.go = "35";
		break;

		case "35":
			dialog.text = "And what is that on your arm?";
			link.l1 = "Nothing ...(pulling the sleeve down over the arm)... A graze, it will heal.";
			link.l1.go = "36";
		break;

		case "36":
			dialog.text = "Captain, that wound must be washed and bound, or it will fester.";
			link.l1 = "Listen, I know what I need and what I do not. Is that clear?";
			link.l1.go = "37";
		break;

		case "37":
			dialog.text = "Quite clear, captain. Heh, that is the second time I have saved your life. You might be a little more civil.";
			link.l1 = "And how did you learn about the atte... about our coming at all?";
			link.l1.go = "38";
		break;

		case "38":
			dialog.text = "How did we learn of the attempt on Lanven and the governor? Easily. A man came running to Lanven and told him everything. \nStefan passed it on to me - luckily I was in the residence at that hour, speaking with the governor. I set the ambush, and no sooner were we ready than you appeared with Clive and his band.";
			link.l1 = "But if you knew an attempt was being prepared, you must have known of my part in it as well.";
			link.l1.go = "39";
		break;

		case "39":
			dialog.text = "We did know of your part, captain. Or rather, that you had been forced into it. We were sure you were still on our side. As you see, we were not mistaken.";
			link.l1 = "...(dryly)... Sure, were they. And what made you so sure?";
			link.l1.go = "40";
		break;

		case "40":
			dialog.text = "Two things, captain. The man who informed Lanven described your conduct in the tavern of La Vega. He said that as you went out you murmured, half to yourself: 'I must stop them, I cannot let this happen'.";
			link.l1 = "I do not recall saying anything of the sort.";
			link.l1.go = "41";
		break;

		case "41":
			dialog.text = "You said it without thinking and may not have noticed. It is as well that Clive's hearing is not as keen as our informant's.";
			link.l1 = "You mentioned two things. What is the second?";
			link.l1.go = "42";
		break;

		case "42":
			dialog.text = "The other is your debt, captain. You owe Lanven your life, and therefore you were bound to try to prevent the attempt on him and on the governor.";
			link.l1 = "True enough, I did try. But you got there first.";
			link.l1.go = "43";
		break;

		case "43":
			dialog.text = "Very well, captain. Tell Lanven the operation was a success and that I am waiting for the money he promised.";
			link.l1 = "The money he promised?";
			link.l1.go = "44";
		break;

		case "44":
			dialog.text = "I did not risk my own life and my men's for nothing.";
			link.l1 = "And how much did Lanven promise you for saving me?";
			link.l1.go = "45";
		break;

		case "45":
			dialog.text = "A thousand piastres for each musketeer and twenty-five thousand for myself. Thirty thousand in all. You come dear to Lanven, captain.";
			link.l1 = "I am willing to pay that sum in Lanven's stead, commandant.";
			link.l1.go = "46";
		break;

		case "46":
			dialog.text = "No, no. Lanven promised to pay me, so he will pay - that is his principle. Even if you gave me a hundred thousand now, he would still hand over his thirty. On principle: promised is done.";
			link.l1 = "Damn, I quite forgot about the permit! You must give me leave to enter Brian George's house. On Lanven's orders I am to search it.";
			link.l1.go = "50";
		break;

		case "50":
			TakeNItems(PChar, "MysteriousArtefact_Notice_1", 1);
			dialog.text = "...(takes out a paper and writes something)... Here is your permit.";
			link.l1 = "Much obliged, commandant.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactToPortPaxExitTownBattleWithTulipBandaEnd");
		break;

		case "51":
			dialog.text = "Brought the permit?";
			link.l1 = "I did. Here, take it.";
			link.l1.go = "52";
		break;

		case "52":
			TakeNItems(PChar, "MysteriousArtefact_Notice_1", -1);
			dialog.text = "Excellent. Now bring me a permit from Lanven, and I shall let you through.";
			link.l1 = "What?! You said to bring the commandant's permit only. Nobody said a word about Lanven.";
			link.l1.go = "53";
		break;

		case "53":
			dialog.text = "Things have changed. Now a permit from Lanven is wanted as well.";
			link.l1 = "Fine, I shall bring one from Lanven too.";
			link.l1.go = "54";
		break;

		case "54":
			dialog.text = "Captain, I was joking. Nothing more is needed, you may pass ...(turns the key in the lock)...";
			link.l1 = "Take your jokes elsewhere. Thick-headed soldier.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("MysteriousArtefactHouseGeorgeBryanOpen");
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
}
