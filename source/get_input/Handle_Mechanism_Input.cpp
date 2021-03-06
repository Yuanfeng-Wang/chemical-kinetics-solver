/*
 * Handle_Mechanism_Input.cpp
 *
 *  Created on: 11.10.2014
 *      Author: DetlevCM
 */

#include <MyHeaders.h>



/* This void needs to supply
 * 1) Species
 * 2) Thermodynamics
 * 3) Reactions
 * 4) Initial Parameters
 * 5) Extras, e.g. PetroOxyData
 */

//void Handle_Mechanism_Input(
bool Handle_Mechanism_Input(
		string mechanism_filename,
		string initial_conditions_fileaname,
		MechanismData& Reaction_Mechanism,
		//vector< string >& Species,vector< ThermodynamicData >& Thermodynamics,vector< SingleReactionData >& Reactions,
		InitParam& InitialParameters,
		vector< double >& InitialSpeciesConcentration,
		PressureVesselCalc & PetroOxyData
)
{

	/// Stragler Variables
	int Number_Species, Number_Reactions;
	int i;

	// Input File via a stream:
	ifstream DataInputFromFile;

	// Two files I want to open, mechanism data & input data
	//string MechanismData = Filename_Mechanism;
	//string InputData = Initial_Conditions; //"initial.inp";


	// Initial data files are either default or user given:

	// check the existence of the 1st input file - the mechanism
	DataInputFromFile.open(mechanism_filename.c_str());
	if (!DataInputFromFile.is_open()) {
		cout << "Error opening " << mechanism_filename << "\n";
		return false;
	}
	DataInputFromFile.close();


	//bool InputType2 = false;

	// check the existence of the 2nd input file - the input data
	DataInputFromFile.open(initial_conditions_fileaname.c_str());
	if (!DataInputFromFile.is_open()) {
		cout << "Error opening " << initial_conditions_fileaname << "\n";
		return false;
	}
	DataInputFromFile.close();


	// As we now know that the input files exist, let us continue by reading in
	// the species list, thermodynamic data and mechanism



	// Get and store Species Information
	Reaction_Mechanism.Species = Get_Species(mechanism_filename);
	Number_Species = (int)Reaction_Mechanism.Species.size();
	cout << "The Mechanism contains " << Number_Species <<" Species.\n";

	Reaction_Mechanism.Thermodynamics = Get_Thermodynamic_Data(mechanism_filename, Reaction_Mechanism.Species);
	cout << "The Mechanism contains " << Reaction_Mechanism.Thermodynamics.size() << " Thermodynamic Data Entries.\n";

	// Get and store the Reaction Mechanism data
	//Reactions = Read_Reaction_Matrix(mechanism_filename, Species);
	Reaction_Mechanism.Reactions = Get_Reactions(mechanism_filename, Reaction_Mechanism.Species);

	Number_Reactions = (int)Reaction_Mechanism.Reactions.size();
	cout << "The Mechanism contains " << Number_Reactions << " Reactions.\n";



	// did the user request species killing? - If yes, execute
	DataInputFromFile.open("kill.txt");
	if (DataInputFromFile.is_open()) {
		Reduce_Species_Thermo_Mechanism(
				Read_Kill_List("kill.txt", Reaction_Mechanism.Species),
				Reaction_Mechanism.Species,
				Reaction_Mechanism.Thermodynamics,
				Reaction_Mechanism.Reactions
		);
		DataInputFromFile.close();

		// Update
		Number_Species = (int)Reaction_Mechanism.Species.size();
		Number_Reactions = (int)Reaction_Mechanism.Reactions.size();

		WriteReactions("Reactions_After_Species_Removal.txt", Reaction_Mechanism.Species, Reaction_Mechanism.Reactions);
	}

	/*
	for(i=0;i<Number_Reactions;i++)
	{
		if(Reactions[i].IsDuplicate == true)
		{
			cout << Reactions[i].IsDuplicate << "\n";
		}
		else
		{
			cout << false << "\n";
		}
	}//*/

	// Some extra mechanism reduction by picking species
	DataInputFromFile.open("SpeciesPicking.txt");
	if (DataInputFromFile.is_open()) {
		mechanism_picking(
				"SpeciesPicking.txt",
				Reaction_Mechanism.Species,
				Reaction_Mechanism.Thermodynamics,
				Reaction_Mechanism.Reactions
		);

		DataInputFromFile.close();
		WriteReactions("Species_Picked_Reactions.txt", Reaction_Mechanism.Species, Reaction_Mechanism.Reactions);

		Write_Thermodynamic_Data("Species_Picked_Thermo.txt", Reaction_Mechanism.Species, Reaction_Mechanism.Thermodynamics);
		WriteSpecies("Species_Picked_Species.txt", Reaction_Mechanism.Species);

		// Update
		Number_Species = (int) Reaction_Mechanism.Species.size();
		Number_Reactions = (int) Reaction_Mechanism.Reactions.size();
	}

	//InitParam InitialParameters;
	vector < InitSpecies > InitalSpecies;


	Get_Initial_Conditions(
			initial_conditions_fileaname,
			Reaction_Mechanism.Species,
			InitialParameters//,
			//InitalSpecies
	); // new function for improved input reading

	InitalSpecies = InitialParameters.InitialLiquidSpecies; // to keep the legacy functionality

	cout << "Initial concentrations are supplied for " << InitalSpecies.size()
										 << " species as follow:\n";


	/*
	 * Once we have read in all user input and make the dangerous assumption
	 * that it does not contain any errors, we can begin to process it and prepare
	 * it for the integrator
	 */

	// We'll store the initial Concentrations separately so we have a reset parameter for multiple runs
	InitialSpeciesConcentration.clear();
	InitialSpeciesConcentration.resize(Number_Species + 1);
	for (i = 0; i <= Number_Species; i++)
	{
		InitialSpeciesConcentration[i] = 0;
	}

	/*
	 * We have obtained the ID of the initial species when we read in the input data
	 * now it is time to translate this information into initial concentrations
	 *
	 * We MUST consider whether the user desires Gas Phase Kinetics or Liquid Phase
	 * Kinetics - for gas phase kinetics the data is adjusted
	 */

	//*
	if(InitialParameters.GasPhase)
	{

		// pV = nRT -> m^3 by pressure is in Pa - Concentration is mol/L
		double GasPressure, GasVolume;

		double Temperature;

		Temperature = InitialParameters.temperature;
		GasPressure = InitialParameters.GasPhasePressure;
		GasVolume = InitialParameters.GasPhaseVolume;

		vector< double > temp(Number_Species);

		for (i = 0; i < (int) InitalSpecies.size(); i++)
		{
			temp[InitalSpecies[i].SpecID] =
					InitalSpecies[i].SpecConc;
		}

		// total mol in input file:
		double total_mol = 0;
		for (i = 0; i < Number_Species; i++)
		{
			total_mol = total_mol + temp[i];
		}
		// Gas Volume m^3, concentrations in input mol/L * volume for mol count
		//total_mol = (1000*total_mol);
		total_mol = (total_mol);



		double n;
		double R = 8.31451; // Gas Constant

		n = (GasPressure * GasVolume) /(Temperature * R);

		// n is now the amount of moles I should have

		double conversion_factor=1;
		if(n!=total_mol)
		{
			conversion_factor = n/(total_mol); // correct for L
		}

		for (i = 0; i < Number_Species; i++)
		{
			InitialSpeciesConcentration[i] = conversion_factor * temp[i];
			if(temp[i]!=0)
			{
				cout << Reaction_Mechanism.Species[InitalSpecies[i].SpecID] << " " << InitialSpeciesConcentration[i] << "\n";
			}
		}
	}
	else
	{
		for (i = 0; i < (int) InitalSpecies.size(); i++)
		{
			InitialSpeciesConcentration[InitalSpecies[i].SpecID] =
					InitalSpecies[i].SpecConc;
			cout << Reaction_Mechanism.Species[InitalSpecies[i].SpecID] << " " << InitalSpecies[i].SpecConc << "\n";
		}//*/
	}

	// and the Initial Temperature
	InitialSpeciesConcentration[Number_Species] = InitialParameters.temperature; //Input[1][0];
	cout << "\n";


	// Did the user request an irreversible scheme?
	if (InitialParameters.irrev) // contains true of false
	{
		cout << "Transformation to irreversible scheme requested!\n";
		Reaction_Mechanism.Reactions = Make_Irreversible(Reaction_Mechanism.Reactions, Reaction_Mechanism.Thermodynamics, InitialParameters.temperature, 50); // hardcoded to +/- 50K right now
		WriteReactions("irreversible_scheme.txt", Reaction_Mechanism.Species, Reaction_Mechanism.Reactions);
	}



	// End of initial data



	/*
	 * If I am to map species to reduce their number, this needs to be applied
	 * to the reaction matrix, so check if the user has supplied a mapping and if yes,
	 * handle further. Otherwise ignore the remapping logic
	 */

	DataInputFromFile.open("species_mapping.txt");
	if (DataInputFromFile.is_open()) {

		// the species mapping requires an irreversible scheme, check the user has requested that, if not, apply it.
		if(!InitialParameters.irrev) // contains true or false
		{
			printf("Transformation to irreversible scheme required! - This will be applied first. \n");
			Reaction_Mechanism.Reactions = Make_Irreversible(Reaction_Mechanism.Reactions, Reaction_Mechanism.Thermodynamics, InitialParameters.temperature, 50); // hardcoded to +/- 50K for now
			WriteReactions("irreversible_scheme_for_mapping.txt", Reaction_Mechanism.Species, Reaction_Mechanism.Reactions);
		}

		int Number_Species_Classes = 0;

		vector<vector<string> > SpeciesMapping; // user input
		vector<int> SpeciesClassMapping; // remapping key

		/* Handle Input In here if it exists */
		SpeciesMapping = Get_Combine_Species_Mapping("species_mapping.txt");
		cout << "Species Mapping Information read in, species will be lumped. \n";

		/*
		 * By supplying the mapping, the user has to agreed to try and combine the species.
		 * Combining the Species means that the reaction vector is re-mapped
		 */

		SpeciesClassMapping = Map_Species_Classes(SpeciesMapping, Reaction_Mechanism.Species);
		vector<ClassNaming> UserDefinedNames;
		UserDefinedNames = GetSpeciesClassesNames(SpeciesMapping);
		Reaction_Mechanism.Species = RenameSpecies(Reaction_Mechanism.Species, UserDefinedNames, SpeciesClassMapping); // Update Species Names
		Number_Species_Classes = (int)Reaction_Mechanism.Species.size(); // need to resize species count a bit later

		Reaction_Mechanism.Thermodynamics = Process_Thermodynamics_Species_Classes(SpeciesClassMapping, Reaction_Mechanism.Thermodynamics); // create new thermodynamics

		//if(InitialParameters.UseNewLumping)
		// new mapping with average Ea and fitted n & A
		//{
		//	cout << "New parameter estimation method.\n";
		Reaction_Mechanism.Reactions = Process_Reactions_For_Species_Lumping(
				Number_Species_Classes,
				SpeciesClassMapping,
				Reaction_Mechanism.Reactions,
				InitialParameters.temperature,
				InitialParameters.MechanismReduction.UseNewLumping,
				InitialParameters.MechanismReduction.UseFastLumping
		); // produce new reactions
		/*}
		else
			// old mapping with n=0
		{
			cout << "Old parameter estimation method.\n";
			Reactions = Process_Species_Combination_Reactions_v2(
					Number_Species_Classes,
					SpeciesClassMapping,
					Reactions,
					InitialParameters.temperature
			); // produce new reactions
		}//*/
		Write_Thermodynamic_Data("recombined_thermo.txt", Reaction_Mechanism.Species, Reaction_Mechanism.Thermodynamics);
		WriteReactions("recombined_scheme.txt", Reaction_Mechanism.Species, Reaction_Mechanism.Reactions);
		WriteSpecies("recombined_species.txt", Reaction_Mechanism.Species);

		// Correct the initial concentrations
		vector<double> temp; // = SpeciesConcentration;
		temp.resize(Number_Species_Classes + 1);

		for (i = 0; i < Number_Species; i++)
		{
			int SpeciesID = SpeciesClassMapping[i];
			temp[SpeciesID] = temp[SpeciesID] + InitialSpeciesConcentration[i];
		}
		temp[Number_Species_Classes] = InitialSpeciesConcentration[Number_Species]; // reassign initial temperature

		if(InitialParameters.PetroOxy.IsSet)
		{
			InitialParameters.PetroOxy.GasSpecies = SpeciesClassMapping[InitialParameters.PetroOxy.GasSpecies];
		}

		Number_Species = Number_Species_Classes; // Number_Species is accessed later and not re-evaluated
		InitialSpeciesConcentration.clear(); // we overwrite the reactions, so why not the initial concentrations, can be used later for reset
		InitialSpeciesConcentration = temp;

		//cout << "Reduced Reaction matrix consists of " << Reactions.size()
		//	 <<	" reactions and " << Reactions[0].Reactants.size() << " species.\n";

		cout << "\nAfter species lumping, the scheme contains the following counts:\n" <<
				"Species: " << Reaction_Mechanism.Reactions[0].Reactants.size() << "\n" <<
				"Reactions: " << Reaction_Mechanism.Reactions.size() << "\n\n";

	}
	DataInputFromFile.close();




	// did the user request a PetroOxy modification?

	if(InitialParameters.PetroOxy.IsSet)
	{
		// adjust old array
		/* 2002 CODATA values */
		double R = 8.314472e0;
		//double Na = 6.0221415e23;

		// not ideal, but avoids the potentia for any issues
		if(InitialParameters.PetroOxy.SampleSize >= InitialParameters.PetroOxy.VesselSize){
			cout << "Pressure vessel is to small, smaller than or equal to the sample size. \n"
					"Vessel Size is set to 110% of sample size.\n";
			InitialParameters.PetroOxy.VesselSize = 1.1 * InitialParameters.PetroOxy.SampleSize;
			cout << "Adjusted Pressure Vessel Size: " << InitialParameters.PetroOxy.VesselSize << "\n";
		}

		PetroOxyData.SampleSize = InitialParameters.PetroOxy.SampleSize;

		// Gas Phase Volume
		//PetroOxyData.HeadSpaceGas = 22.5*1e-6 - PetroOxyData.SampleSize;
		//cout << InitialParameters.PressureVessel.VesselSize << "\n";
		//PetroOxyData.HeadSpaceGas = InitialParameters.PressureVessel.VesselSize*1e-6 - PetroOxyData.SampleSize;
		PetroOxyData.HeadSpaceGas = InitialParameters.PetroOxy.VesselSize - PetroOxyData.SampleSize;

		// Initial pressure is at 25 degrees celsius
		// n = pV/R/T
		PetroOxyData.HeadSpaceGasMol =
				InitialParameters.PetroOxy.InitPressure*PetroOxyData.HeadSpaceGas/R/298;
		// p = nRT/V
		PetroOxyData.HeadSpaceGasPressure = PetroOxyData.HeadSpaceGasMol*R*InitialParameters.temperature/PetroOxyData.HeadSpaceGas;

		// Solvent Component of Pressure
		PetroOxyData.HeadSpaceSolventComponentPressure = InitialParameters.PetroOxy.MaxPressure-PetroOxyData.HeadSpaceGasPressure;


		// the user is asked to input mol / L at 1atm
		// 100% oxygen assumed - mol/L to mol/m^3 * 1000 - not Done
		// atm to Pa - *101325
		PetroOxyData.HenryConstantk = 101325/InitialParameters.PetroOxy.GasSolubility;
		// k is now Pa mol / L


		// Mod for limitied diffusion
		PetroOxyData.HenryLawDiffusionLimitSet = InitialParameters.PetroOxy.HenryLawDiffusionLimitSet;
		PetroOxyData.HenryLawDiffusionLimit = InitialParameters.PetroOxy.HenryLawDiffusionLimit;
	}





	return true;
}

