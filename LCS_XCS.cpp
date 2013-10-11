/**
==================================

An implementation of the eXtended Classifier System (XCS). 

Tim Lukins (2002)

==================================

Can also be built to test standalone with:

	g++ -g -DTEST -o xcs LCS_XCS.cpp

And memory tested then with:

  valgrind --tool=memcheck -v ./xcs

Followed by this (to see where exactly):

	valgrind --tool=memcheck --leak-check=full --track-origins=yes -v ./xcs

==================================
*/

#pragma warning(disable: 4786) // Freakin' M$!
#include "LCS_XCS.h"

// In global namespace... polymorphism of ostream << and istream >>

/**
* Output of classifier (serialize):
*/ 

ostream& operator<<(ostream& stream, const LCS::XCS::Classifier& cl) {

	// Convert condition to string representation...
	string con;
	for (int c=0;c<cl._condition.size(); c++)
		switch(cl._condition[c]) {
			case LCS::XCS::Classifier::ONCE: con+="1"; break;
			case LCS::XCS::Classifier::ZERO: con+="0"; break;
			case LCS::XCS::Classifier::DONT: con+="#"; break;
	}

	return stream
		<< con << " "
		<< (LCS::XCS::Action)cl._action << " " 
		<< (double)cl._prediction << " "
		<< (double)cl._error << " "
		<< (double)cl._fitness << " "
		<< (unsigned long)cl._experience << " "
		<< (unsigned long)cl._timestamp << " "
		<< (unsigned long)cl._actionsetsize << " "
		<< (unsigned long)cl._numerosity;
}

/**
 * Input of classifier:
 */ 

istream& operator>>(istream& stream, LCS::XCS::Classifier& cl) {

	string c;
	LCS::XCS::Action a;
	double p;
	double e;
	double f;
	unsigned long x;
	unsigned long t;
	unsigned long s;
	unsigned long n;

	// TODO: check no end of line...

	stream >> c >>  a >>  p >> e >> f >>  x  >> t  >> s  >> n;

	// Break up condition...
	vector<LCS::XCS::Classifier::Symbol> con;
	for (int sym=0;sym<c.size(); sym++) {
		switch(c[sym]) {
			case '1': con.push_back(LCS::XCS::Classifier::ONCE); break;
			case '0': con.push_back(LCS::XCS::Classifier::ZERO); break;
			case '#': con.push_back(LCS::XCS::Classifier::DONT); break;
			default: break; // ignore
		}
	}

	// Assign values to classifier...
	cl.assign(con,a,p,e,f,x,t,s,n); // TODO: t=sys->_time ?

	return stream;
}

using namespace LCS;

////////////////////////////////////////// XCS class:

/**
 * Constructor:
 */

XCS::XCS(XCS::Actions acts) {

	// The actions available...
	_actions = acts;

	// Sensible default values...
	BETA	= 0.15;
	GAMMA	= 0,71;
	ALPHA	= 0.1;
	ERROR	= 10;   // ideally 1% of max reward
 	VAL		= 5;
	EPSILON	= 0.5;  // But depending on problem
	N		= 1000;
	MU		= 0.03; // 0.01-0.05
	XU		= 0.6;  // 0.5-1.0
	SIGMA	= 0.1;
	PHASH	= 0.33;
	THETAGA = 30;   // 25-50
	THETADEL= 20;
	THETASUB= 20;
	THETAACT= _actions.size();    // Number of possible actions.

	// Default control options...
	doSubsumption	= true; // Subsumption is applied both to action set and GA
	doLearning		= true; // Create an action set, update it, and apply GA

	// Reset internal metrics...
	_time		= 0; // Total epochs running
	_reinforced = 0; // Epochs when reinforced

	// Initialize random number generator...
	_seed = ((long)(time(NULL)%10000+1));

}

/**
 * Destructor:
 */

XCS::~XCS() {

	clear();
}

/**
 * Control Methods:
 */

void XCS::learningOn() {
	doLearning = true;
}

void XCS::learningOff() {
	doLearning = false;
}

void XCS::subsumptionOn() {
	doSubsumption = true;
}

void XCS::subsumptionOff() {
	doSubsumption = false;
}

/**
 * Query Methods:
 */

long XCS::populationSize() {
	return _population.size();
}

double XCS::internalPerformance() {
	return (double)_reinforced/_time;
}

unsigned long XCS::currentTime() {
	return _time;
}

/**
 * Load:
 */

void XCS::load(istream& from) {

	// Read whole line first...
	string line;
	while(from >> line) {
		// Create new classifier and renew it with data...
		Classifier* renewed = new Classifier(this); // ALLOC
		istringstream fline(line);
		//fline >> *renewed;
	}
}

/**
 * Save:
 */

void XCS::save(ostream& to) {

	// Every classifier on a seperate line...
	for (ClassifierIter cl =_population.begin();cl!=_population.end(); cl++)
		to << *(*cl) << endl;
}

/**
 * Clear:
 */

void XCS::clear() {

	// Free memory first...
	for (ClassifierIter cl = _population.begin();cl!=_population.end(); cl++)
		delete(*cl); // DEALLOC

	_population.clear(); 
}

/**
 * Step:
 */
/*
void XCS::step() {

	// Increment time...
	_time++;

	// Have a look at what's out there...
	_percept = _environment->perceive();

	// Generate match set as a result (covering if necessary)...
	generateMatchset();

	// Select an action from the match set (based on prediction values)...
	selectAction();

	// Execute action and collect reward...
	if (_reward = _environment->act(_proposed >0)) _reinforced++;

	if (doLearning) {

		// Generate the action set...
		generateActionSet();

		// Update action set with payoff...
		updatePrediction();

		// Possibly run GA on action set (but actually effect overall population)... 
		applyGA();
	}
}
*/

/**
 * Take action
 */

XCS::Action XCS::act(XCS::Perception state) {

	// Increment time...
	_time++;

	// Have a look at what's out there...
	_percept = state;

	// Generate match set as a result (covering if necessary)...
	generateMatchset();

	// Select an action from the match set (based on prediction values)...
	selectAction();

	// Return it
	return _proposed;
}

/**
 * Update reward
 */

void XCS::update(XCS::Reward by) {
	
	// Collect reward...
	_reward = by;	
	if (_reward>0) _reinforced++;

	if (doLearning) {

		// Generate the action set...
		generateActionSet();

		// Update action set with payoff...
		updatePrediction();

		// Possibly run GA on action set (but actually effect overall population)... 
		applyGA();
	}
}

/**
 * Generate Match Set:
 */

void XCS::generateMatchset() {
	
	// Record count of proposed actions...
	Actions proposals;

	// First empty any from last time (only pointers)...
	_matchset.clear();

	// While matchset is empty...
	while (_matchset.empty()) {

		// For each classifier in the population...
		for (ClassifierIter cl = _population.begin();cl!=_population.end(); cl++) {

			// If classifer matches situation...
			if ((*cl)->matches(_percept)) {

				// Add it to matchset...
				_matchset.push_back(*cl);

				// Record actions proposed by matching classifiers (no duplicates)...
				ActionIter already = find(proposals.begin(),proposals.end(),(*cl)->_action);
				if (already==proposals.end()) proposals.push_back((*cl)->_action);
			}
		}

		// If the number of different actions in the matchset is low...
		if (proposals.size()<THETAACT && doLearning) {

			// Generate covering classifier in population...
			Classifier* response = new Classifier(this); // ALLOC
		  // Was trying this...	
			//Classifier* response = make_shared<Classifier>(this); // ALLOC

			// Find random action not present in Matchset
			Action rand = _actions[0];
			do {
				rand = _actions[(int)(drand()*100)%_actions.size()];
				ActionIter found = find(proposals.begin(),proposals.end(),rand);
				if (found==proposals.end()) break;
			} while(true);
			// Actually cover...
			response->cover(_percept,rand);
			_population.push_back(response);

			// Cull population...
			deleteFromPopulation();

			// Empty the match set (so we'll go round again)...
			_matchset.empty();
			
		}
	}
}

/**
 * Select Action:
 */

void XCS::selectAction() {

	//  Initialliaze prediction array...
	map<Action,double> predictions;
	map<Action,double> fitsum;
	ActionIter act;
	for (act=_actions.begin(); act!=_actions.end(); act++) {
		predictions[*act]=0.0;
		fitsum[*act]=0.0;
	}

	// Generate the prediction array for actions in the match set...
	for (ClassifierIter cl = _matchset.begin();cl!=_matchset.end(); cl++) {
		predictions[(*cl)->_action] += (*cl)->_prediction * (*cl)->_fitness;
		fitsum[(*cl)->_action] += (*cl)->_fitness;
	}

	// Normalize...
	for (act=_actions.begin(); act!=_actions.end(); act++)
		if (fitsum[*act]!=0.0) predictions[*act]=predictions[*act]/fitsum[*act];

	// Decide what action to take (explore or exploit)...
	if (drand()<EPSILON) {
		// Randomly chose an action whose prediction is not zero...
		long tries = 0; // Put in to avoid indefinite comparison to INF or IND
		while(true) {
			_proposed = _actions[(int)(drand()*100)%_actions.size()];
			if (predictions[_proposed]!=0.0 || (tries++ > 100)) break;
		}
	}
	else {
		// Find the action with the highest prediction...
		double highest = 0.0;
		for (act=_actions.begin(); act!=_actions.end(); act++) { 
			if (predictions[*act]>highest) {
				highest = predictions[*act];
				_proposed = *act;
			}
		}
	}
}

/**
 * Generate Action Set:
 */

void XCS::generateActionSet() {

	// First empty any from last time (only pointers)...
	_actionset.clear();

	// Iterate thro' matchset...
	for (ClassifierIter cl = _matchset.begin();cl!=_matchset.end(); cl++) {

		// Test to see if action is the same as proposed...
		if ((*cl)->_action == _proposed)
			_actionset.push_back(*cl);
	}
}

/**
 * Update predictions:
 */

void XCS::updatePrediction() {

        // Total up the numerosity values for all in actionset...
	long sigman = 0;
	for (ClassifierIter cn = _actionset.begin();cn!=_actionset.end(); cn++)
		sigman += (*cn)->_numerosity;

	// Every classifier in the actionset...
	for (ClassifierIter cl = _actionset.begin();cl!=_actionset.end(); cl++) {

		// Increase experience...
		(*cl)->_experience++;

		// Update actual prediction values, error and action set size estimate...
		if ((*cl)->_experience < 1/BETA) { 
			(*cl)->_prediction		+= (_reward - (*cl)->_prediction) / (*cl)->_experience;
			(*cl)->_error			+= (abs(_reward - (*cl)->_prediction) - (*cl)->_error) / (*cl)->_experience ;
			(*cl)->_actionsetsize	+= (sigman - (*cl)->_actionsetsize) / (*cl)->_experience;
		}
		else {
			(*cl)->_prediction		+= BETA * (_reward - (*cl)->_prediction);
			(*cl)->_error			+= BETA * (abs(_reward - (*cl)->_prediction));
			(*cl)->_actionsetsize	        += (long)BETA * (sigman - (*cl)->_actionsetsize);
		}
	}

	// Update fitness as well...
	updateFitness();

	// Check and maybe do some subsumption...
	if (doSubsumption) doActionSetSubsumption();
}

/**
 * Update fitness...
 */

void XCS::updateFitness() {

	// Init accuracy sum and accuracy vector...
	long accsum = 0;
	map<Classifier*,double> accuracy;

	// Every classifier in the actionset...
	for (ClassifierIter cl = _actionset.begin();cl!=_actionset.end(); cl++) {

		if ((*cl)->_error < ERROR) 
			accuracy[*cl] = 1;
		else
			accuracy[*cl] = ALPHA * pow((*cl)->_error / ERROR,-VAL);

		accsum += (long)accuracy[*cl] * (*cl)->_numerosity;

	}

	// Then normalize...

	for (ClassifierIter no = _actionset.begin();no!=_actionset.end(); no++)
		(*no)->_fitness += BETA * (accuracy[*no] * (*no)->_numerosity / accsum - (*no)->_fitness);

}

/**
 * Apply GA:
  */

void XCS::applyGA() {
	
	// Calculate timestamp average and numerosity...
	long sumtimestamp = 0;
	long sumnumerosity = 0;
	for (ClassifierIter sm = _actionset.begin();sm!=_actionset.end(); sm++) {
		sumtimestamp += (*sm)->_timestamp;
		sumnumerosity += (*sm)->_numerosity;
	}

	// See if the GA actually needs to be applied...
	double avgtime = sumtimestamp;
	if (sumnumerosity!=0) sumtimestamp/=sumnumerosity; // Shouldn't happen really....
	
	if ((_time - avgtime) > THETAGA) {

		// It's GA time!
		for (ClassifierIter cl = _actionset.begin();cl!=_actionset.end(); cl++) {

			// Update timestamp of this classifier...
			(*cl)->_timestamp = _time;
	
			// Select two parents...
			Classifier* pa = selectOffspring();
			Classifier* ma = selectOffspring();
/*
			if (pa==NULL || ma==NULL)
				return; // Population is too small I think...
			
			// Copy some new, inexperienced children...
			Classifier* jack = new Classifier(*pa);//->copy();
			Classifier* jill = new Classifier(*ma);//->copy();
			jack->_numerosity = jill->_numerosity = 1;
			jack->_experience = jill->_experience = 0;
			bool doJack = false;
			bool doJill = false;

			// Possibly some crossover...
			if (drand() < XU) {
				applyCrossover(jack,jill);
				jack->_prediction = jill->_prediction = (pa->_prediction + ma->_prediction)/2;
				jack->_error = jill->_error = (pa->_error + ma->_error)/2;
				jack->_fitness = jill->_fitness = (pa->_fitness + ma->_fitness)/2;
			}

			// Possibly some mutation...
			if (drand() < MU)
				applyMutation(jack);
			if (drand() < MU)
				applyMutation(jill);

			// Check for subsumption...
			if (doSubsumption) {
				if (doesSubsume(pa,jack) || doesSubsume(pa,jill)) {
					pa->_numerosity++;
				}
				else {
					insertIntoPopulation(jack);
					doJack = true;
				}
				if (doesSubsume(ma,jack) || doesSubsume(ma,jill)) {
					ma->_numerosity++;
				}
				else {
					insertIntoPopulation(jill);
					doJill = true;
				}
				if (!doJill)	
					delete(jill); // We're not going to use jill
				if (!doJack)	
					delete(jack); // We're not going to use jack
			}
			// Add kids to population anyway...
			else {
				insertIntoPopulation(jack);
				insertIntoPopulation(jill);
			}
			*/
			// Cull the population if necessary...
			deleteFromPopulation();
		}
	}
}

/**
 * Select offspring
 */

XCS::Classifier* XCS::selectOffspring() {

	// If no classifiers, return null
	
	if (_actionset.size()==0)
		return NULL;

	// Total up all fitness...
	double fitsum = 0.0;
	for (ClassifierIter f = _actionset.begin();f!=_actionset.end(); f++) 
		fitsum += (*f)->_fitness;

	// Select "roulette" point...
	double spin = drand() * fitsum;

	// Return first classifier above that point...
	fitsum = 0.0;
	ClassifierIter cl;
	for (cl=_actionset.begin();cl!=_actionset.end(); cl++) {
		fitsum += (*cl)->_fitness;
		if (fitsum > spin) break;
	}

	return *cl;
}

/**
 * Apply Crossover:
 */

void XCS::applyCrossover(Classifier* one, Classifier* two){

	// Establish two crossover points...
	long from = (long)(drand()*(one->_condition.size()+1));
	long to = from + (long)(drand()*((one->_condition.size()-from)+1));

	// Switch over the symbols at those positions...
	for (int i=from; i<to; i++) {
		XCS::Classifier::Symbol temp	= one->_condition[i];
		one->_condition[i]				= two->_condition[i];
		two->_condition[i]				= temp;
	}
}

/**
 * Apply Mutation:
 */

void XCS::applyMutation(Classifier* cl){

	// Consider every position along the condition...
	for (int i=0; i<cl->_condition.size(); i++) {

		// If mutatation occurs...
		if (drand() < MU) {

			// Restricted change, geared to matching current perception...
			if (cl->_condition[i] == XCS::Classifier::DONT) //or 'HASH'
				cl->_condition[i] = (XCS::Classifier::Symbol)_percept[i];
			else
				cl->_condition[i] = XCS::Classifier::DONT;
		}
	}

	// Furthermore, the action may change as well...
	if (drand() < MU) 
		cl->_action = _actions[(int)(drand()*_actions.size())];
}

/**
 * Insert Into Population:
 */

void XCS::insertIntoPopulation(Classifier* poss){

	// Check to see if there already exists such a classifier...
	for (ClassifierIter cl = _actionset.begin();cl!=_actionset.end(); cl++) {

		if ((*cl)->_condition == poss->_condition && (*cl)->_action  == poss->_action) {
			(*cl)->_numerosity++;
			return; // i.e. Don't add poss
		}
	}

	// It must be new - so OK to add...
	_population.push_back(poss);

}

/**
 * Delete From Population:
 */

void XCS::deleteFromPopulation(){

	// Check first to see if beneath max size anyway...
	long sumnum = 0;
	double sumfit = 0.0;
	for (ClassifierIter n = _actionset.begin();n!=_actionset.end(); n++) {
		sumnum += (*n)->_numerosity;
		sumfit += (*n)->_fitness;
	}
	if (sumnum < N) return;

	// If OK - establish distribution of "vote"...
	double votesum = 0.0;
	double avgfitinpop = sumfit / sumnum;
	for (ClassifierIter v = _actionset.begin();v!=_actionset.end(); v++)
		votesum += deletionVote(*v,avgfitinpop);

	// Spin and see...
	double spin = drand() * votesum;

	// Find where that falls...
	votesum = 0.0;
	for (ClassifierIter a = _actionset.begin();a!=_actionset.end(); a++) {
		votesum += deletionVote(*a,avgfitinpop);
		if (votesum > spin) {

			// Reduce numerosity (it's "weight" in voting)...
			(*a)->_numerosity--;
			
			// And remove it completely (if numerosity zero)...
			if ((*a)->_numerosity==0)
				//delete(*a); // DEALLOC
				_population.erase(a); 

			// Only update that one classifier...
			return;
		}
	}
}

/**
 * Deletion Vote:
 */

double XCS::deletionVote(Classifier* cl, double avgfit) {

	double vote = cl->_actionsetsize * cl->_numerosity;

	// Modify vote weighting accordingly...
	if (cl->_experience > THETADEL && cl->_fitness/cl->_numerosity <  SIGMA * avgfit)
		vote *= avgfit/(cl->_fitness/cl->_numerosity);

	return vote;
}

/**
 * Do Action Set Subsumption:
 */

void XCS::doActionSetSubsumption() {

	// Find  the most general classifier in the action set...
	Classifier* cl = NULL;
	int c;
	for (ClassifierIter a = _actionset.begin();a!=_actionset.end(); a++) {
		if (couldSubsume(*a)) {
			if (cl == NULL ||
				countGenerality(*a) > countGenerality(cl) ||
				((countGenerality(*a) == countGenerality(cl)) && 
				(drand() < 0.5))) {
				cl = (*a);
			}
		}
	}

	// Eliminate any classifiers subsumed by this one...
	if (cl!=NULL) {
		for (ClassifierIter c = _actionset.begin();c!=_actionset.end(); c++) {
			if (moreGeneral(cl,*c)) {
				cl->_numerosity += (*c)->_numerosity;
				ClassifierIter found = find(_population.begin(),_population.end(),*c);
				//if (found==_population.end())
				_population.erase(found); // TODO Check iterator OK in this case...
				_actionset.erase(c);
				//delete(*c); // DEALLOC
			}
		}
	}
}

/**
 * Could Subsume:
 */

bool XCS::couldSubsume(Classifier* cl) {

	if (cl->_experience > THETASUB && cl->_error < ERROR) return true;
	else return false;
}

/**
 * Count Generality:
 */

long XCS::countGenerality(Classifier* cl) {

	long total = 0;
	// Total number of times DONT(HASH) occurs...
	for (int s=0; s<cl->_condition.size(); s++)
		if (cl->_condition[s]==XCS::Classifier::DONT) total++;
	return total;
}

/**
 * More General:
 */

bool XCS::moreGeneral(Classifier* gen, Classifier* spec) {

	// Predicate...
	if (countGenerality(gen) <= countGenerality(spec)) return false;
	else {
		for (int i=0; i<gen->_condition.size(); i++) {
			if (gen->_condition[i]!=XCS::Classifier::DONT &&
				gen->_condition[i]==spec->_condition[i])
				return false;
		}
	}
	return true;
}

/**
 * Does Subsume:
 */

bool XCS::doesSubsume(Classifier* sub, Classifier* tos) {

	// Predicate...
	if (sub->_action == tos->_action && couldSubsume(sub) && moreGeneral(sub,tos))
		return true;
	else
		return false;
}

/**
 * Random number generator returning double over standard distribution from 0->1
 */
	
double XCS::drand() {

  long M = 2147483647;
  long A = 16807; 
  long Q = M/A;
  long R = M%A;
  long lo,hi,test;
  
  hi   = _seed / Q;
  lo   = _seed % Q;
  test = A*lo - R*hi;
  
  if (test>0)
    _seed = test;
  else
    _seed = test+M;

  return (double)(_seed)/M;

}

#ifdef TEST

/**
 * Testing with XOR problem
 *
 * NOTE: handy to then run this in valgrind as:
 *    > valgrind --tool=memcheck --leak-check=full ./xcs
 * to reveal the exact location of any leaks (if you build with -g).
 * See also:
 *   http://stackoverflow.com/questions/3982036/how-can-i-use-valgrind-with-python-c-extensions
 */

void XCS::test() {
	
	cout << "+++START+++" << endl;

	_actions.clear();
	_actions.push_back(0); // i.e. output from xor
	_actions.push_back(1);

	THETAACT=_actions.size(); // Directly set

	// If you do this 1000 times you should flush memory issues...

	for (int i=0;i<=1000;i++) {

		cout << "Time = " << _time++ << endl;;

		//cout << "Drand = " << drand() << endl;

		int situation = ((int)(drand()*100))%4; // i.e. 00,01,10, or 11

		_percept.clear();
		switch(situation) {
		case 0: {
			cout << "Percept -> [00]" << endl;
			_percept.push_back(0);
			_percept.push_back(0);
			break;
		}
		case 1: {
			cout << "Percept -> [01]" << endl;
			_percept.push_back(0);
			_percept.push_back(1);
			break;
		}
		case 2: {
			cout << "Percept -> [10]" << endl;
			_percept.push_back(1);
			_percept.push_back(0);
			break;
		}
		case 3: {
			cout << "Percept -> [11]" << endl;
			_percept.push_back(1);
			_percept.push_back(1);
			break;
		}
		}

		ClassifierIter cl;

		cout << "+++ Generating match set +++" << endl;

		generateMatchset();

		for (cl = _matchset.begin();cl!=_matchset.end(); cl++)
			cout << "In matchset: " << *(*cl) << endl;

		cout << "+++ Selecting action +++" << endl;

		selectAction();

		cout << "Action chosen: " << _proposed << endl;

		// Calculate if correct action chosen...

		if (_proposed==1 && (situation==1 || situation==2)) // i.e. 1 on 10 or 01
			_reward = 1000;
		else if (_proposed==0 && (situation==0 || situation==3)) // i.e. 0 on 00 or 11
			_reward = 1000;
		else
			_reward = -1000;

		cout << "Reward of " << _reward << endl;

		if (_reward>0) _reinforced++;

		cout << "+++ Generating action set +++" << endl;

		generateActionSet();

		for (cl = _actionset.begin();cl!=_actionset.end(); cl++)
			cout << "In actionset: " << *(*cl) << endl;

		cout << "+++ Updating predictions +++" << endl;

		updatePrediction();

		for (cl = _population.begin();cl!=_population.end(); cl++)
			cout << "In population: " << *(*cl) << endl;

		cout << "+++ Applying GA +++" << endl;

		applyGA();

		for (cl = _population.begin();cl!=_population.end(); cl++)
			cout << "In population: " << *(*cl) << endl;

		cout << "+++ Pop size = " << populationSize() << " +++" << endl;
		cout << "+++ Int Perf = " << internalPerformance() << " +++" << endl; 

	}

	cout << "+++END+++" << endl;
}

int main(int argv,char** argc) { 
	XCS::Actions acts; 
	XCS dummy(acts); 
	dummy.test(); 
}

#endif 

/////////////////////////////////////// Classifier Class:

/**
 * Constructor:
 */

XCS::Classifier::Classifier(XCS* sys) {

	// Handle to the system this classifier is part of...
	_system = sys;

	// Initialize condition (ahead of covering with specifics)...
	for (int i=0; i<_system->_percept.size(); i++)
		_condition.push_back(DONT);

}

/**
 * Copy:
 */

//XCS::Classifier* XCS::Classifier::copy() {
XCS::Classifier::Classifier(const XCS::Classifier& other) {

	//XCS::Classifier* dup = new XCS::Classifier(this->_system); // ALLOC

	this->_system;

	// ERRORS HERE ON COPY...
	//std::copy(other._condition.begin(),other._condition.end(),this->_condition.begin());
  this->_condition = other._condition;//vector<Symbol>(other._condition);	
  /////////////////////////////	
	this->_action = other._action;
	this->_prediction = other._prediction;
	this->_error = other._error;
	this->_fitness = other._fitness;
	this->_experience = other._experience;
	this->_timestamp = other._timestamp;
	this->_actionsetsize = other._actionsetsize;
	this->_numerosity = other._numerosity;
}


/**
 * Destructor:
 */

XCS::Classifier::~Classifier() {

	this->_condition.clear();
}

/**
 * Matches:
 */

bool XCS::Classifier::matches(Perception sigma) {

	// For each attribute of the condition...	
	for (int x=0; x<_condition.size(); x++) {

		// Check to see if we don't care, or if it equals...
		if (_condition[x]!=DONT && _condition[x]!=(Symbol)sigma[x]) return false;
	}

	return true;
}

/**
 * Cover:
 */

void XCS::Classifier::cover(Perception sigma, Action act) {

	// Build condition...
	for (int x=0; x<sigma.size(); x++) {
		if (_system->drand()<_system->PHASH) _condition[x]=DONT;
		else _condition[x]=(Symbol)sigma[x];
	}

	_action			= act;
	_prediction		= 0.01;
	_error			= 0.01;
	_fitness		= 0.01;
	_experience		= 0;
	_timestamp		= _system->_time;
	_actionsetsize	= 1;
	_numerosity		= 1;


}

/**
 * Assign:
 */

void XCS::Classifier::assign(vector<Symbol> c, Action a, double p, double e, double f, unsigned long x, unsigned long t, unsigned long s, unsigned long n) {

	_condition = c;
	_action = a;
	_prediction = p;
	_error = e;
	_fitness = f;
	_experience = x;
	_timestamp = t;
	_actionsetsize = s;
	_numerosity = n;
}

