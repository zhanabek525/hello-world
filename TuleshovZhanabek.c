#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	char name[20];
	char city[20];
	int points;
	int goalFor;
	int goalAgainst;
} Team;

typedef struct {
	int matchDay;
	Team *host;
	Team *guest;
	int hostScore;
	int guestScore;
}Match;

int initTeams(Team teams[32]){
	int i = 0, teamCount = 0;
	FILE *finput = fopen("teams.txt", "r");
	if (finput == NULL){
		printf("\nThe error in accessing the file!\n\n");
		return 0;
	}
	while(!feof(finput)){
		fscanf(finput, "%s %s", teams[i].name, teams[i].city);
		i++;
	}
	fclose(finput);
	teamCount = i;
	for (i = 0; i != teamCount; ++i){
		teams[i].points = 0;
		teams[i].goalFor = 0;
		teams[i].goalAgainst = 0;
	}
	return teamCount;
}

int addResults(int n, Team teams[32]){
	FILE *input = fopen("match-results.txt", "r");
	if (input == NULL){
		printf("\nThe error in accessing the file!\n\n");
	}
	Team *host, *guest;
	int i, k, hscore, gscore, nMatches = 0;
	char name1[20], city1[20], name2[20], city2[20];
	while(!feof(input)){
		strcpy(name1, "\0");
		strcpy(name2, "\0");
		fscanf(input, "%i %s %s %i %i %s %s", &k, name1, city1, &hscore, &gscore, name2, city2);
		host = NULL;
		for (i = 0; i != n; ++i){ //this loop covers the data about the host team
			if (strcmp(name1, teams[i].name) == 0){
				if (strcmp(city1, teams[i].city) == 0){
					host = &teams[i]; //we make variable 'host' point to the host team to be able to use it later
					break; //we break since we found a team
				}// if the cities don't match, it keeps searching
			}//if the names don't match, it keeps searching
		}
		if (host == NULL){
			continue; // this means that the team is not found in the first n elements of array
		}

		guest = NULL;
		for (i = 0; i != n; ++i){
			if (strcmp(name2, teams[i].name) == 0){
				if (strcmp(city2, teams[i].city) == 0){
					guest = &teams[i];
					break;
				}
			}
		}
		if (guest == NULL){
			continue;
		}
		host->goalFor += hscore;
		guest->goalFor += gscore;
		host->goalAgainst += gscore;
		guest->goalAgainst += hscore;
		if (hscore > gscore){
			host->points += 3;
		} else if (gscore > hscore){
			guest->points += 3;
		} else {
			host->points += 1;
			guest->points += 1;
		}
		++nMatches;
	}

	fclose(input);
	return nMatches;
}

Team* printStandings(int n, Team teams[32], char fileName[20]){
	putchar('\n');
	printf("%-19s %7s %7s %7s %7s\n", "TEAM", "P", "GF", "GA", "GD");
	int i;
	for (i = 0; i != n; ++i){
		printf("%-19s %7i %7i %7i ", teams[i].name, teams[i].points, teams[i].goalFor, teams[i].goalAgainst);
		int difference = teams[i].goalFor - teams[i].goalAgainst;
		if (difference > 0){
			if (difference < 10){
				printf("%6s%i", "+", difference);
			} else if (difference < 100){
				printf("%5s%i", "+", difference);
			} else {
				printf("%4s%i", "+", difference);
			}
		} else if (difference < 0){
			difference = teams[i].goalAgainst - teams[i].goalFor;
			if (difference < 10){
				printf("%6s%i", "-", difference);
			} else if (difference < 100){
				printf("%5s%i", "-", difference);
			} else {
				printf("%4s%i", "-", difference);
			}
		} else {
			printf("%7s", "0");
		}
		putchar('\n');
	}

	FILE *file = fopen(fileName, "w");
	if (file == NULL){
		printf("\nThe error in accessing the file!\n\n");
	}
	fprintf(file, "%-19s %7s %7s %7s %7s\n", "TEAM", "P", "GF", "GA", "GD");
	for (i = 0; i != n; ++i){
		fprintf(file, "%-19s %7i %7i %7i ", teams[i].name, teams[i].points, teams[i].goalFor, teams[i].goalAgainst);
		int difference = teams[i].goalFor - teams[i].goalAgainst;  //This portion of code attempts to print out the goal difference in a way that is shown in the task
		if (difference > 0){
			if (difference < 10){   // if the difference is lower than 10 then allocate the corresponding spaces
				fprintf(file, "%6s%i", "+", difference);
			} else if (difference < 100){  // if the difference is lower than 100 but greater than 10 inclusively then allocate the corresponding spaces
				fprintf(file, "%5s%i", "+", difference);
			} else {  // the code assumes that the difference should not be greater than or equal to 1000 otherwise the table might be a bit distorted
				fprintf(file, "%4s%i", "+", difference);
			}
		} else if (difference < 0){
			difference = teams[i].goalAgainst - teams[i].goalFor;
			if (difference < 10){
				fprintf(file, "%6s%i", "-", difference);
			} else if (difference < 100){
				fprintf(file, "%5s%i", "-", difference);
			} else {
				fprintf(file, "%4s%i", "-", difference);
			}
		} else {
			fprintf(file, "%7s", "0");
		} // until here: this is the end of the code printing goal difference
		putc('\n', file);
	}
	fclose(file);

	int hPoints = 0, champIndex;
	for (i = 0; i != n; ++i){  //this loop identifies the highest number of points
		if (hPoints < teams[i].points){
			hPoints = teams[i].points;
		}
	}
	int count = 0;
	for (i = 0; i != n; ++i){  //this loop identifies the number of teams with the same number of points
		if (hPoints == teams[i].points){
			++count;
			champIndex = i;  // if there is only one champion then it stores its index, if there are many with same score then it stores the last one
		}
	}
	int highestGD = 0;
	if(count > 1){ // we enter this if statement only if there are more than one team with the same number of points
		int samePoints[count];  //an array that stores the indices of teams with the same number of points
		int k;
		for (i = 0, k = 0; i != n; i++){ // each element of samePoints now stores the indices
			if (hPoints == teams[i].points){
				samePoints[k] = i;
				++k;
			}
		}
		putchar('\n');
		for (i = 0; i != count; i++){  //this loop identifies the highest goal difference
			int gd = teams[samePoints[i]].goalFor - teams[samePoints[i]].goalAgainst;
			if (highestGD < gd){
				highestGD = gd;
			}
		}
		int count2 = 0;
		for (i = 0; i != count; ++i){
			int gd = teams[samePoints[i]].goalFor - teams[samePoints[i]].goalAgainst;
			if (highestGD == gd){
				count2++;
				champIndex = samePoints[i]; // if there is only one champion then it stores its index, if there are many with same goal difference then it stores the last one
			}
		}
		if (count2 > 1){
			int sameGD[count2];  // stores the indices of those teams with the same number of points and goal difference
			for (i = 0, k = 0; i != count; ++i){
				int gd = teams[samePoints[i]].goalFor - teams[samePoints[i]].goalAgainst;
				if (highestGD == gd){
					sameGD[k] = samePoints[i];
					++k;
				}
			}
			int highGoalFor = 0;
			for (i = 0; i != count2; ++i){
				if (highGoalFor < teams[sameGD[i]].goalFor){
					highGoalFor = teams[sameGD[i]].goalFor;
				}
			}
			int count3 = 0;
			for (i = 0; i != count2; ++i){
				if (highGoalFor == teams[sameGD[i]].goalFor){
					count3++;
					champIndex = sameGD[i];  // if there is only one champion then it stores its index, if there are many with same goal for then it stores the last one
				}
			}
			if (count3 > 1){
				return NULL;
			} else {
				return &teams[champIndex];
			}
		} else {
			return &teams[champIndex];
		}
	} else {
		return &teams[champIndex];
	}

}

Match* storeResult(Team *host, Team *guest){
	int countLine = 0, matchDay, h, g;
	char name1[20], city1[20];
	char name2[20], city2[20];
	FILE *file = fopen("match-results.txt", "r");
	if (file == NULL){
		printf("\nThe error in accessing the file!\n\n");
	}
	Match* output = (Match*) malloc(sizeof(Match));
	while(!feof(file)){
		strcpy(name1, "\0");
		strcpy(name2, "\0");
		fscanf(file, "%i %s %s %i %i %s %s", &matchDay, name1, city1, &h, &g, name2, city2);
		if (!strcmp(name1, host->name)){
			if (!strcmp(city1, host->city)){
				if (!strcmp(name2, guest->name)){
					if (!strcmp(city2, guest->city)){
						break;
					}
				}
			}
		}
		++countLine;
	}

	if(feof(file)){
		return NULL;
	}
	fclose(file);

	file = fopen("match-results.txt", "r");
	if (file == NULL){
		printf("\nError with accessing the file!\n\n");
	}
	int i;
	for (i = 0; i != countLine; ++i){
		fscanf(file, "%i %s %s %i %i %s %s", &matchDay, name1, city1, &h, &g, name2, city2);
	}
	fscanf(file, "%i %s %s %i %i %s %s", &matchDay, name1, city1, &h, &g, name2, city2);
	output->matchDay = matchDay;
	output->host = host;
	output->guest = guest;
	output->hostScore = h;
	output->guestScore = g;
	fclose(file);
	return output;
}

Team* printOrderedStandings(int n, Team teams[32], char fileName[20]){
	Team copy[32];  //to keep teams not sorted we create a new array
	int i;
	for (i = 0; i != n; ++i){
		copy[i] = teams[i];
	}

	int k;
	for (k = n - 1; k != 0; --k){
		for (i = 0; i != k; ++i){
			if (copy[i].points < copy[i+1].points){
				Team temp = copy[i];
				copy[i] = copy[i+1];
				copy[i+1] = temp;
			} else if (copy[i].points == copy[i+1].points){
				int gd1 = copy[i].goalFor - copy[i].goalAgainst;
				int gd2 = copy[i+1].goalFor - copy[i+1].goalAgainst;
				if (gd1 < gd2){
					Team temp = copy[i];
					copy[i] = copy[i+1];
					copy[i+1] = temp;
				} else if (gd1 == gd2){
					if (copy[i].goalFor < copy[i+1].goalFor){
						Team temp = copy[i];
						copy[i] = copy[i+1];
						copy[i+1] = temp;
					} else if (copy[i].goalFor == copy[i+1].goalFor){
						if (strcmp(copy[i].name, copy[i+1].name) > 0){
							Team temp = copy[i];
							copy[i] = copy[i+1];
							copy[i+1] = temp;
						}
					}
				}
			}
		}
	}

	putchar('\n');
	printf("%-19s %7s %7s %7s %7s\n", "TEAM", "P", "GF", "GA", "GD");
	for (i = 0; i != n; ++i){
		printf("%-19s %7i %7i %7i ", copy[i].name, copy[i].points, copy[i].goalFor, copy[i].goalAgainst);
		int difference = copy[i].goalFor - copy[i].goalAgainst;
		if (difference > 0){
			if (difference < 10){
				printf("%6s%i", "+", difference);
			} else if (difference < 100){
				printf("%5s%i", "+", difference);
			} else {
				printf("%4s%i", "+", difference);
			}
		} else if (difference < 0){
			difference = copy[i].goalAgainst - copy[i].goalFor;
			if (difference < 10){
				printf("%6s%i", "-", difference);
			} else if (difference < 100){
				printf("%5s%i", "-", difference);
			} else {
				printf("%4s%i", "-", difference);
			}
		} else {
			printf("%7s", "0");
		}
		putchar('\n');
	}

	FILE *file = fopen(fileName, "w");
	if (file == NULL){
		printf("\nThe error in accessing the file!\n\n");
	}
	fprintf(file, "%-19s %7s %7s %7s %7s\n", "TEAM", "P", "GF", "GA", "GD");
	for (i = 0; i != n; ++i){
		fprintf(file, "%-19s %7i %7i %7i ", copy[i].name, copy[i].points, copy[i].goalFor, copy[i].goalAgainst);
		int difference = copy[i].goalFor - copy[i].goalAgainst;  //This portion of code attempts to print out the goal difference in a way that is shown in the task
		if (difference > 0){
			if (difference < 10){   // if the difference is lower than 10 then allocate the corresponding spaces
				fprintf(file, "%6s%i", "+", difference);
			} else if (difference < 100){  // if the difference is lower than 100 but greater than 10 inclusively then allocate the corresponding spaces
				fprintf(file, "%5s%i", "+", difference);
			} else {  // the code assumes that the difference should not be greater than or equal to 1000 otherwise the table might be a bit distorted
				fprintf(file, "%4s%i", "+", difference);
			}
		} else if (difference < 0){
			difference = copy[i].goalAgainst - copy[i].goalFor;
			if (difference < 10){
				fprintf(file, "%6s%i", "-", difference);
			} else if (difference < 100){
				fprintf(file, "%5s%i", "-", difference);
			} else {
				fprintf(file, "%4s%i", "-", difference);
			}
		} else {
			fprintf(file, "%7s", "0");
		} // until here: this is the end of the code printing goal difference
		putc('\n', file);
	}
	fclose(file);

	
	


	if (copy[0].points == copy[1].points){
		int gd1 = copy[0].goalFor - copy[0].goalAgainst;
		int gd2 = copy[1].goalFor - copy[1].goalAgainst;
		if (gd1 == gd2){
			if (copy[0].goalFor == copy[1].goalFor){
				return NULL;
			}
		}
	}
	
	for (i = 0; i != n; ++i){
		if (strcmp(copy[0].name, teams[i].name) == 0){
			if (strcmp(copy[0].city, teams[i].city) == 0){
				return &teams[i];
			}
		} 
	}

}

int main(void){
	setvbuf(stdout, NULL, _IONBF, 0);
	char firstName[] = "Zhanabek";
	char lastName[] = "Tuleshov";
	char studentID[] = "201825700";
	printf("Student:  %s %s\n\n", firstName, lastName);

	//Task 1

	Team teams[32];
	int n = initTeams(teams);

	/*   This code outputs the initial content of the array 'teams' when it is all zeros with the name and city
	int i = 0;
	for (i = 0; i != n; i++){
		printf("%s %s %i %i %i\n", teams[i].name, teams[i].city, teams[i].points, teams[i].goalFor, teams[i].goalAgainst);
	}
	printf("\n");
	*/


	//Task 2
	
	//addResults() adds results of the matches between the first n teams in the array 'teams'

	//Since addResults() function changes the content of teams, each test for addResults() should be done separately and the code in line 459 should be commented out so that the test doesn't interfere with that line which is by default not commented in this program

	/*   Test case when 3 is passed for 'n'
	int nMatches = addResults(3, teams);
	int i = 0;
	for (i = 0; i != n; i++){
		printf("%-15s %-15s %5i %5i %5i\n", teams[i].name, teams[i].city, teams[i].points, teams[i].goalFor, teams[i].goalAgainst);
	}
	printf("\nThe number of matches processed: %i \n", nMatches);
	*/

	/*  Test case when 6 is passed for 'n'
	int nMatches = addResults(6, teams);
	int i = 0;
	for (i = 0; i != n; i++){
		printf("%-15s %-15s %5i %5i %5i\n", teams[i].name, teams[i].city, teams[i].points, teams[i].goalFor, teams[i].goalAgainst);
	}
	printf("\nThe number of matches processed: %i \n", nMatches);
	*/

	/*   Test case when 9 is passed for 'n
	int nMatches = addResults(9, teams);
	int i = 0;
	for (i = 0; i != n; i++){
		printf("%-15s %-15s %5i %5i %5i\n", teams[i].name, teams[i].city, teams[i].points, teams[i].goalFor, teams[i].goalAgainst);
	}
	printf("\nThe number of matches processed: %i \n", nMatches);
	*/

	//   Test case when the total number of teams is passed
	int matchNo = addResults(n, teams);

	/* this code tests whether it returns the number of matches stored in match-results.txt
	printf("\nThe number of matches processed: %i\n\n", matchNo);
	*/


	//Task 3

	/*    This portion of code was used to test that all 4 conditions are met

	//1st requirement
	// here we have only one team (team3) with the highest number of points and it is outputed
	Team test1[4] = {{"team1", "city1", 25, 20, 15},
					 {"team2", "city2", 18, 15, 19},
					 {"team3", "city3", 28, 42, 16},
	  	  	  	  	 {"team4", "city4", 23, 32, 12}};
	Team *testP1 = printStandings(4, test1, "test1.txt");
	if (testP1 == NULL){
		printf("No champion!\n\n");
	} else{
		printf("\nThe champion is %s\n\n", testP1->name);
	}
	//end

	//2nd requirement
	//here we have two teams (team1 and team3) with highest number of points
	//but only one (team 3) with greater goal difference
	Team test2[4] = {{"team1", "city1", 28, 20, 15},
					 {"team2", "city2", 18, 15, 19},
					 {"team3", "city3", 28, 42, 16},
	  	  	  	  	 {"team4", "city4", 23, 32, 12}};
	Team *testP2 = printStandings(4, test2, "test2.txt");
	if (testP2 == NULL){
		printf("No champion!\n\n");
	} else{
		printf("\nThe champion is %s\n\n", testP2->name);
	}
	//end

	//3rd requirement
	//here we have three teams with the same highest score (28)
	//and two of them with the same highest goal difference
	//and only one (team2) with the highest goal for number
	Team test3[4] = {{"team1", "city1", 28, 20, 15},
					 {"team2", "city2", 28, 39, 19},
					 {"team3", "city3", 22, 42, 16},
	  	  	  	  	 {"team4", "city4", 28, 32, 12}};
	Team *testP3 = printStandings(4, test3, "test3.txt");
	if (testP3 == NULL){
		printf("No champion!\n\n");
	} else{
		printf("\nThe champion is %s\n\n", testP3->name);
	}
	//end

	//4th requirement
	//here we have all 4 teams with the same score
	//2 of them with the same highest goal difference and same highest goal for number
	//it means that there are more than 1 champion, as a result the function returns NULL
	Team test4[4] = {{"team1", "city1", 28, 32, 12},
					 {"team2", "city2", 28, 24, 19},
					 {"team3", "city3", 28, 19, 19},
	  	  	  	  	 {"team4", "city4", 28, 32, 12}};
	Team *testP4 = printStandings(4, test4, "test4.txt");
	if (testP4 == NULL){
		printf("No champion!\n\n");
	} else{
		printf("\nThe champion is %s\n\n", testP4->name);
	}
	//end

	*/

	Team* champion = printStandings(n, teams, "KazPL");

	/*    This code outputs the champion if there is a champion, if not, it outputs 'no champion'
	if (champion == NULL){
		printf("No champion!");
	}
	printf("\nThe champions is %s\n", champion->name);
	*/


	//Task 4

	/*  Test cases for Task 4
	//The very first match in match-results.txt, that is Ordabasy vs Shakhter
	Match *firstM = storeResult(&teams[0], &teams[10]);
	if (firstM == NULL){
		printf("\nThere is no such a match between %s and %s\n", teams[0].name, teams[10].name);
	}else {
		printf("\n\nMatch day: %i\n%s %s %i - %i %s %s\n", firstM->matchDay, firstM->host->name,
				firstM->host->city, firstM->hostScore, firstM->guestScore, firstM->guest->name, firstM->guest->city);
	}
	free(firstM);

	//The very last match in match-results.txt, that is between Taraz and Irtysh
	Match *lastM = storeResult(&teams[11], &teams[9]);
	if (lastM == NULL){
		printf("\nThere is no such a match between %s and %s\n", teams[11].name, teams[9].name);
	} else {
		printf("\n\nMatch day: %i\n%s %s %i - %i %s %s\n", lastM->matchDay, lastM->host->name,
				lastM->host->city, lastM->hostScore, lastM->guestScore, lastM->guest->name, lastM->guest->city);
	}
	free(lastM);

	//AstVsTaraz is just a arbitrarily chosen match from match-results.txt
	Match* AstVsTaraz = storeResult(&teams[3], &teams[11]);
	if (AstVsTaraz == NULL){
		printf("\nThere is no such a match between %s and %s\n", teams[3].name, teams[11].name);
	} else {
		printf("\n\nMatch day: %i\n%s %s %i - %i %s %s\n", AstVsTaraz->matchDay, AstVsTaraz->host->name,
				AstVsTaraz->host->city, AstVsTaraz->hostScore, AstVsTaraz->guestScore, AstVsTaraz->guest->name, AstVsTaraz->guest->city);
	}
	free(AstVsTaraz);


	//here we search the match between non existing team in the array 'teams' and the real team from the file
	//it should output NULL pointer
	Team non_existing = {"Real Madrid", "Madrid", 0, 0, 0};
	Match* matchP = storeResult(&non_existing, &teams[5]);
	if (matchP == NULL){
		printf("\nThere is no such a match between %s and %s\n", non_existing.name, teams[5].name);
	}else{
		printf("\n\nMatch day: %i\n%s %s %i - %i %s %s\n", matchP->matchDay, matchP->host->name,
				matchP->host->city, matchP->hostScore, matchP->guestScore, matchP->guest->name, matchP->guest->city);
	}
	free(matchP);
	*/


	//Task 5

	//  This test uses the array 'teams' and sorts it
	Team* champ1 = printOrderedStandings(n, teams, "Ordered KazPL");

	/* Ordered Table Comments:
	 *
	 * From the ordered table there are three teams with 35 points:
	 * They are: Astana, Irtysh, and Kairat
	 * They have the identical goal difference and number of scored goals
	 * As a result, they are alphabetically ordered
	 * Overall, all teams in the table are sorted according to their points, then goal difference, and then scored goals
	 */

	/* Outputs the champion
	if (champ1 == NULL){
		printf("\nThere is no champion\n\n");
	} else {
		printf("\nThe champion is %s\n\n", champ1->name);
	}
	*/

	/* 4 cases for Task 5
	
	printf("\n\nTest cases for Task 5\n\n");
	 
	//Here the table is sorted only according to teams' points despite the original order is not sorted
	Team test5[] = {{"Zoo", "Zoo", 6, 6, 5},
				  {"Sts", "Sts", 8, 12, 7},
	 	 	 	  {"Kok", "Kok", 12, 16, 7},
				  {"Bcb", "Bcb", 5, 6, 12}};
	Team *testP5 = printOrderedStandings(4, test5, "test5.txt");
	if (test5 == NULL){
		printf("\nThere is no champion\n\n");
	} else {
		printf("\nThe champion: %s\n\n", testP5->name);
	}
	
	//Here three teams have the same points (8), and when they are sorted they are sorted according to their goal difference
	Team test6[] = {{"Zoo", "Zoo", 8, 9, 4},
				  {"Sts", "Sts", 8, 12, 9},
	 	 	 	  {"Bcb", "Kok", 12, 16, 5},
				  {"Kok", "Bcb", 8, 6, 6}};
	Team *testP6 = printOrderedStandings(4, test6, "test6.txt");
	if (test6 == NULL){
		printf("\nThere is no champion\n\n");
	} else {
		printf("\nThe champion: %s\n\n", testP6->name);
	}
	//Here three teams have the same points, the same goal difference but different number of goals scored, goals scored determines the order of those 3 teams
	Team test7[] = {{"Zoo", "Zoo", 12, 6, 1},
				  {"Sts", "Sts", 12, 14, 9},
	 	 	 	  {"Kok", "Kok", 12, 12, 7},
				  {"Bcb", "Bcb", 5, 6, 11}};
	Team *testP7 = printOrderedStandings(4, test7, "test7.txt");
	if (test7 == NULL){
		printf("\nThere is no champion\n\n");
	} else {
		printf("\nThe champion: %s\n\n", testP7->name);
	}
	
	//This tests if the 4th requirement works (alphabetical order)
	//All 4 teams have identical parameters, alphabetic order determines the way they are ordered
	Team test8[] = {{"Zoo", "Zoo", 5, 6, 5},
				  {"Sts", "Sts", 5, 6, 5},
	 	 	 	  {"Kok", "Kok", 5, 6, 5},
				  {"Bcb", "Bcb", 5, 6, 5}};
	Team *testP8 = printOrderedStandings(4, test8, "test8.txt");
	if (test8 == NULL){
		printf("\nThere is no champion\n\n");
	} else {
		printf("\nThe champion: %s\n\n", testP8->name);
	}
	*/

	return 0;
}

