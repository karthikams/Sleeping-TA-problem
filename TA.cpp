/*
 * TA.cpp
 *
 *  Created on: Nov 10, 2017
 *      Author: karthika
 */

#include<cstdlib>
#include<iostream>
#include<pthread.h>
#include<vector>
#include<unistd.h>
#include<semaphore.h>

using namespace std;

void *ta_function(void *arg);
void *student_function(void *arg);
const unsigned int empty_chairs = 3;

//add the variable for sleep

pthread_mutex_t ta_mutex;
pthread_mutex_t student_mutex;
pthread_t ta;
sem_t number_of_chairs;

const int maximum_programming_time = 5;
const int ta_student_teaching_time = 3;


int main (){
	int number_of_students, i,k;
	cout << "Enter the number of students" << endl;
	cin >> number_of_students;
	pthread_t student[number_of_students];
	int student_index[number_of_students];

	//Initializing the mutex and semaphore
	pthread_mutex_init(&ta_mutex, 0);
	pthread_mutex_init(&student_mutex,0);
	sem_init(&number_of_chairs, 0, empty_chairs);

	pthread_mutex_lock(&ta_mutex);
	// creating  a TA thread
	i = pthread_create(&ta,NULL,ta_function,NULL);
	sleep(2);
	if(i) cout<< "Error in creating ta" << endl;

	//creating threads for students
	for(int j = 0; j<number_of_students; j++) {
		student_index[j] = j;
		k = pthread_create(&student[j],NULL,student_function, (void*)&student_index[j]);
		if(k){
			cout << ("Error while creating student ") << j << endl;
			exit(-1);
		}
	}
	// Joining the threads
	pthread_join(ta,NULL);
	cout << "TA joined" << endl << flush;
	for(int j = 0; j<number_of_students;j++){
		pthread_join(student[j],NULL);
		cout << "thread joined for student " << j << endl << flush;
	}
}

void *ta_function(void *arg) {
	//cout << "TA is sleeping " << endl << flush ;
	int chairs;
	while(true){
		sem_getvalue(&number_of_chairs, &chairs);
		if(chairs == 3 ){
			cout << "TA is sleeping. Wake me if you need help" << endl << flush;
		}
		pthread_mutex_lock(&ta_mutex);
		cout << "TA is talking to the student " << endl << flush;
		sleep(ta_student_teaching_time);
		pthread_mutex_unlock(&student_mutex);
	}
}

void *student_function(void *arg){
	int chairs;
	int programming_time;
	while(true){
		programming_time = rand()%maximum_programming_time;
		cout << "Student "<< *(int*)arg << " is programming for "
				<< programming_time << endl << flush;
		sleep(programming_time);
		sem_getvalue(&number_of_chairs,&chairs);

		if(chairs == 0){
			continue;
		}
		sem_wait(&number_of_chairs);
		cout<< "Student "<< *(int*)arg << " is now sitting on a chair" << endl << flush;
		if (pthread_mutex_trylock(&student_mutex) == 0) {
			//we are the first student, have to wake up the TA
			if (chairs == 3) {
				//We are the first student, have to wake the TA up
				cout << "Student "<< *(int*)arg << " is waking up the TA" << endl << flush;
			}
		} else {
			// Someone else is talking to TA wait outside office
			cout << "Student "<< *(int*)arg << " is waiting outside the office" << endl << flush;
			pthread_mutex_lock(&student_mutex);
		}
		sem_post(&number_of_chairs);
		pthread_mutex_unlock(&ta_mutex);
		cout <<"Student "<< *(int*)arg << " is talking to TA"<< endl << flush;
		sleep(ta_student_teaching_time);
		return 0;
	}

}
