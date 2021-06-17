// Library for the YSBL program Privateer (PRogramatic Identification of Various Anomalies Toothsome Entities Experience in Refinement)
// Licence: LGPL - Please check Licence.txt for details.
//
// 2013-
// York Structural Biology Laboratory
// The University of York


#include <Python.h>
#include <pybind11_json/pybind11_json.hpp> //nlohman::json
#include <nlohmann/json.hpp> //nlohman::json
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <string>
#include <exception>
#include <algorithm>
#include "clipper-glyco.h"
#include "privateer-lib.h"
#include "privateer-xray.h"
#include "privateer-cryo_em.h"
#include "privateer-dbquery.h"
#include "privateer-parallelism.h"


#ifndef PRIVATEER_PYANALYSIS_H_INCLUDED
#define PRIVATEER_PYANALYSIS_H_INCLUDED

namespace privateer {

  namespace pyanalysis {

    // TODO: add [] operator for these classes.

    class GlycanStructure;
    class CarbohydrateStructure;
    class XRayData;
    class CryoEMData;
    class OfflineDatabase;

    class GlycosylationComposition 
    {
      public:
      // need to add constructor with both model and experimental data too. 
        GlycosylationComposition() { };
        GlycosylationComposition(std::string& path_to_model_file, std::string expression_system) {
          this->read_from_file ( path_to_model_file, expression_system );
        };
        GlycosylationComposition(std::string& path_to_model_file, std::string& path_to_mtz_file, std::string& input_column_fobs_user, int nThreads, float ipradius, std::string expression_system);
        GlycosylationComposition(std::string& path_to_model_file, std::string& path_to_mrc_file, float resolution, int nThreads, float ipradius, std::string expression_system);
        ~GlycosylationComposition() { };
        void read_from_file( std::string path_to_model_file, std::string expression_system );
        void initialize_summary_of_detected_glycans();

        std::string get_path_of_model_file_used ( ) { return path_to_model_file; };
        std::string get_expression_system_used ( ) { return expression_system; };
        int get_number_of_glycan_chains_detected ( ) { return numberOfGlycanChains; };
        
        pybind11::list get_summary_of_detected_glycans () { return glycosylationSummary; };
        
        GlycanStructure get_glycan(const int id);

        pybind11::list get_ligands() { return ligands; };

        void update_with_experimental_data(privateer::pyanalysis::XRayData& xray_data);
        void update_with_experimental_data(privateer::pyanalysis::CryoEMData& cryoem_data);
        bool check_if_updated_with_experimental_data() { return updatedWithExperimentalData; };
      private:
        clipper::MGlycology mgl;
        std::vector<std::pair<clipper::String, clipper::MSugar>> ligandList;
        std::vector<std::pair<clipper::String, clipper::MSugar>> ligandsOnly;
        std::string path_to_model_file;
        std::string expression_system;
        int numberOfGlycanChains;
        pybind11::list glycosylationSummary;
        pybind11::list glycans;
        pybind11::list ligands;

        bool updatedWithExperimentalData;
    };

    class GlycanStructure 
    {
      public:
        GlycanStructure() { };
        GlycanStructure(const clipper::MGlycology& mgl, const int glycanID, privateer::pyanalysis::GlycosylationComposition& parentGlycosylationComposition){
          this->updatedWithExperimentalData = false;
          this->pyinit ( mgl, glycanID, parentGlycosylationComposition );
        };
        GlycanStructure(const clipper::MGlycology& mgl, const int glycanID, privateer::pyanalysis::GlycosylationComposition& parentGlycosylationComposition, std::vector<std::pair< clipper::String , clipper::MSugar> >& finalLigandList){
          this->updatedWithExperimentalData = true;
          this->pyinitWithExperimentalData ( mgl, glycanID, parentGlycosylationComposition, finalLigandList );
        };
        ~GlycanStructure() { };
        void pyinit ( const clipper::MGlycology& mgl, const int glycanID, privateer::pyanalysis::GlycosylationComposition& parentGlycosylationComposition);
        void pyinitWithExperimentalData (const clipper::MGlycology& mgl, const int glycanID, privateer::pyanalysis::GlycosylationComposition& parentGlycosylationComposition, std::vector<std::pair< clipper::String , clipper::MSugar> >& finalLigandList);
        void initialize_summary_of_glycan();
        // void update_summary_of_glycan(); // could be a private method after database stuff. 
        
        int get_glycan_id( ) const { return glycanID; };
        int get_total_number_of_sugars( ) { return numberOfSugars; };
        std::string get_wurcs_notation( ) { return glycanWURCS; };
        pybind11::list get_unique_monosaccharide_codes( ) { return uniqueMonosaccharides; };
        int get_total_of_glycosidic_bonds( ) { return numberOfGlycosidicBonds; };
        std::string get_glycosylation_type( ) { return glycosylationType; };
        pybind11::dict get_root_info( ) { return rootSummary; };
        pybind11::dict get_protein_glycan_linkage_torsions( ) { return protein_glycan_linkage_torsion; };

        pybind11::dict get_glycan_summary( ) { return glycanSummary; };

        CarbohydrateStructure get_monosaccharide(const int glycanID);
        pybind11::list get_all_monosaccharides( ) { return sugars; };

        // void query_offline_database( const OfflineDatabase& importedDatabase, bool returnClosestMatches, bool returnAllPossiblePermutations, int nThreads );
        // pybind11::list return_permutations_of_glycan(bool returnAllPossiblePermutations, int nThreads) // would return a list of permutated GlycanStructure Objects. Interesting to see how much the memory will have blown up.
        // void generateSNFG();

        bool check_if_updated_with_experimental_data() { return updatedWithExperimentalData; };
      private:
        privateer::pyanalysis::GlycosylationComposition parentGlycosylation;
        clipper::MGlycan glycan;
        std::vector<clipper::MSugar> sugars_in_glycan;
        
        int glycanID;
        int numberOfSugars;
        std::string glycanWURCS;
        pybind11::list uniqueMonosaccharides;
        int numberOfGlycosidicBonds;
        std::string glycosylationType;
        pybind11::dict rootSummary;
        pybind11::dict protein_glycan_linkage_torsion;
        pybind11::dict glycanSummary;
        pybind11::list sugars;

        bool updatedWithExperimentalData;
    };
  
    class CarbohydrateStructure 
    {
      public:
        CarbohydrateStructure() { };
        CarbohydrateStructure(clipper::MGlycan& mglycan, const int sugarID, const int glycanID, privateer::pyanalysis::GlycosylationComposition& parentGlycosylationComposition, privateer::pyanalysis::GlycanStructure& parentGlycanStructure){
          this->updatedWithExperimentalData = false;
          this->pyinit ( mglycan, sugarID, glycanID, parentGlycosylationComposition, parentGlycanStructure );
        };
        CarbohydrateStructure(clipper::MGlycan& mglycan, const int sugarID, const int glycanID, privateer::pyanalysis::GlycosylationComposition& parentGlycosylationComposition, privateer::pyanalysis::GlycanStructure& parentGlycanStructure, std::vector<clipper::MSugar>& list_of_sugars){
          this->updatedWithExperimentalData = true;
          this->pyinitWithExperimentalData ( mglycan, sugarID, glycanID, parentGlycosylationComposition, parentGlycanStructure, list_of_sugars );
        };
        // bool updatedWithExperimentalData
        CarbohydrateStructure(const int sugarID, std::vector<std::pair<clipper::String, clipper::MSugar>>& inputSugarList, privateer::pyanalysis::GlycosylationComposition& parentGlycosylationComposition, bool updatedWithExperimentalData){
          this->updatedWithExperimentalData = updatedWithExperimentalData;
          this->pyinitLigand( sugarID, inputSugarList, parentGlycosylationComposition );
        };
        ~CarbohydrateStructure() { };
        
        void pyinit (clipper::MGlycan& mglycan, const int sugarID, const int glycanID, privateer::pyanalysis::GlycosylationComposition& parentGlycosylationComposition, privateer::pyanalysis::GlycanStructure& parentGlycanStructure);
        void pyinitLigand ( const int sugarID, std::vector<std::pair<clipper::String, clipper::MSugar>>& inputSugarList, privateer::pyanalysis::GlycosylationComposition& parentGlycosylationComposition);
        void pyinitWithExperimentalData (clipper::MGlycan& mglycan, const int sugarID, const int glycanID, privateer::pyanalysis::GlycosylationComposition& parentGlycosylationComposition, privateer::pyanalysis::GlycanStructure& parentGlycanStructure, std::vector<clipper::MSugar>& list_of_sugars);

        void initialize_summary_of_sugar();

        bool operator==(const CarbohydrateStructure& inputSugar) const { return (sugar_pdb_id == inputSugar.get_sugar_pdb_id() && sugar_pdb_chain == inputSugar.get_sugar_pdb_chain()); }

        pybind11::dict get_sugar_summary( ) { return sugarSummary; };
        
        int get_sugar_id( ) const { return sugarID; };
        int get_glycan_id( ) const { return glycanID; };
        int get_sugar_pdb_id() const { return sugar_pdb_id; };
        std::string get_sugar_pdb_chain() const { return sugar_pdb_chain; };

        std::string get_conformation_name() { return sugar_conformation_name; };
        std::string get_conformation_name_iupac() { return sugar_conformation_name_iupac; };
        float get_puckering_amplitude() { return sugar_puckering_amplitude; };
        std::string get_anomer() { return sugar_anomer; };
        std::string get_handedness() { return sugar_handedness; };
        std::string get_denomination() { return sugar_denomination; };
        int get_ring_cardinality() { return sugar_ring_cardinality; };
        pybind11::list get_cremer_pople_params() { return sugar_cremer_pople_params; };
        bool is_sane() { return sugar_sane; };
        std::string get_privateer_diagnostic() { return privateer_diagnostic; };
        std::string get_name_full() { return sugar_name_full; };
        std::string get_name_short() { return sugar_name_short; };
        std::string get_type() { return sugar_type; };
        pybind11::list get_ring_angles() { return sugar_ring_angles; };
        pybind11::list get_ring_bonds() { return sugar_ring_bonds; };
        pybind11::list get_ring_torsion() { return sugar_ring_torsion; };
        float get_ring_bond_rmsd() { return sugar_ring_bond_rmsd; };
        float get_ring_angle_rmsd() { return sugar_ring_angle_rmsd; };
        float get_bfactor() { return sugar_bfactor; };
        bool is_supported() { return sugar_supported; };
        bool ok_with_ring() { return sugar_diag_ring; };
        bool ok_with_bonds_rmsd() { return sugar_diag_bonds_rmsd; };
        bool ok_with_angles_rmsd() { return sugar_diag_angles_rmsd; };
        bool ok_with_anomer() { return sugar_diag_anomer; };
        bool ok_with_chirality() { return sugar_diag_chirality; };
        bool ok_with_conformation() { return sugar_diag_conformation; };
        bool ok_with_puckering() { return sugar_diag_puckering; };
        float get_sugar_rscc() { return sugar_rscc; };
        float get_sugar_accum() { return sugar_accum; };
        bool get_sugar_occupancy_check() { return sugar_occupancy_check; }; 
        std::string get_glycosylation_context() { return sugar_context; };

        void set_sugar_rscc(float input_sugar_rscc) { sugar_rscc = input_sugar_rscc; };
        void set_sugar_accum(float input_sugar_accum) { sugar_accum = input_sugar_accum; };
        void set_sugar_occupancy_check(bool input_sugar_occupancy_check) { sugar_occupancy_check = input_sugar_occupancy_check; };

        bool check_if_updated_with_experimental_data() { return updatedWithExperimentalData; };
      private:
        privateer::pyanalysis::GlycosylationComposition parentGlycosylation;
        privateer::pyanalysis::GlycanStructure parentGlycanStructure;
        clipper::MSugar sugar;
        clipper::MGlycan parentGlycan;

        pybind11::dict sugarSummary;

        int sugarID;
        int glycanID;
        int sugar_pdb_id;
        std::string sugar_pdb_chain;

        int sugar_conformation_code;
        std::string sugar_conformation_name;
        std::string sugar_conformation_name_iupac;
        float sugar_puckering_amplitude;
        std::string sugar_anomer;
        std::string sugar_handedness;
        std::string sugar_denomination;
        int         sugar_ring_cardinality;
        // const clipper::Coord_orth& sugar_centre; // leave this for future implementation.
        // const clipper::Vec3<ftype>& sugar_mean_plane; // leave this for future implementation.
        pybind11::list sugar_cremer_pople_params; // std::vector<ftype>
        bool sugar_sane;
        std::string privateer_diagnostic;
        std::string sugar_name_full;
        std::string sugar_name_short;
        std::string sugar_type;
        pybind11::list sugar_ring_angles; // std::vector<ftype>
        pybind11::list sugar_ring_bonds; // std::vector<ftype>
        pybind11::list sugar_ring_torsion; // std::vector<ftype>
        float sugar_ring_bond_rmsd;
        float sugar_ring_angle_rmsd;
        float sugar_bfactor;
        bool sugar_supported;
        bool sugar_diag_ring;
        bool sugar_diag_bonds_rmsd;
        bool sugar_diag_angles_rmsd;
        bool sugar_diag_anomer;
        bool sugar_diag_chirality;
        bool sugar_diag_conformation;
        bool sugar_diag_puckering;
        float sugar_rscc = -1; // need to develop setter method as in privateer.cpp
        float sugar_accum = -1; // need to develop setter method as in privateer.cpp
        bool sugar_occupancy_check = false; // need to develop setter method as in privateer.cpp
        std::string sugar_context;

        bool updatedWithExperimentalData;
    };

    class XRayData 
    {
      public:
        XRayData() { };
        XRayData(std::string& path_to_mtz_file, std::string& path_to_model_file, std::string& input_column_fobs_user, float ipradius, int nThreads) {
          this->read_from_file ( path_to_mtz_file, path_to_model_file, input_column_fobs_user, ipradius, nThreads);
        };
        ~XRayData() { };
        void read_from_file( std::string& path_to_mtz_file, std::string& path_to_model_file, std::string& input_column_fobs_user, float ipradius, int nThreads);
        pybind11::list get_sugar_summary_with_experimental_data() { return sugar_summary_of_experimental_data; };
        pybind11::list get_ligand_summary_with_experimental_data() { return ligand_summary_of_experimental_data; };
        void print_cpp_console_output_summary() 
        { 
          int pos_slash = path_to_model_file.rfind("/");
          clipper::String path_to_model_file_clipper = path_to_model_file;
          privateer::util::print_monosaccharide_summary_python (false, false, pos_slash, false, finalLigandList, hklinfo, path_to_model_file_clipper); 
        };
        

        std::vector<std::pair< clipper::String , clipper::MSugar> > get_finalLigandList() { return finalLigandList; } // only c++
        std::vector<std::pair< clipper::String , clipper::MSugar> > get_finalLigandOnly() { return final_LigandsOnly; } // only c++
      private:
        std::string path_to_model_file;
        clipper::MiniMol mmol;
        clipper::HKL_info hklinfo;
        clipper::String input_column_fobs;
        
        std::vector<std::pair< clipper::String , clipper::MSugar> > finalLigandList;
        std::vector<std::pair<clipper::String, clipper::MSugar>> final_LigandsOnly;
        pybind11::list sugar_summary_of_experimental_data;
        pybind11::list ligand_summary_of_experimental_data;

        // private methods
        pybind11::list generate_sugar_experimental_data_summary(std::vector<std::pair< clipper::String , clipper::MSugar>>& finalLigandList);
    };

    class CryoEMData 
    {
      public:
        CryoEMData() { };
        CryoEMData(std::string& path_to_mrc_file, std::string& path_to_model_file, float resolution, float ipradius, int nThreads) {
          this->read_from_file ( path_to_mrc_file, path_to_model_file, resolution, ipradius, nThreads);
        };
        ~CryoEMData() { };
        void read_from_file( std::string& path_to_mrc_file, std::string& path_to_model_file, float resolution, float ipradius, int nThreads);
        pybind11::list get_sugar_summary_with_experimental_data() { return sugar_summary_of_experimental_data; };
        pybind11::list get_ligand_summary_with_experimental_data() { return ligand_summary_of_experimental_data; };
        void print_cpp_console_output_summary() 
        { 
          int pos_slash = path_to_model_file.rfind("/");
          clipper::String path_to_model_file_clipper = path_to_model_file;
          privateer::util::print_monosaccharide_summary_python (false, false, pos_slash, true, finalLigandList, hklinfo, path_to_model_file_clipper); 
        };
        

        std::vector<std::pair< clipper::String , clipper::MSugar> > get_finalLigandList() { return finalLigandList; } // only c++
        std::vector<std::pair< clipper::String , clipper::MSugar> > get_finalLigandOnly() { return final_LigandsOnly; } // only c++
      private:
        std::string path_to_model_file;
        float resolution;
        clipper::MiniMol mmol;
        clipper::HKL_info hklinfo;
        
        std::vector<std::pair< clipper::String , clipper::MSugar> > finalLigandList;
        std::vector<std::pair<clipper::String, clipper::MSugar>> final_LigandsOnly;
        pybind11::list sugar_summary_of_experimental_data;
        pybind11::list ligand_summary_of_experimental_data;

        // private methods
        pybind11::list generate_sugar_experimental_data_summary(std::vector<std::pair< clipper::String , clipper::MSugar>>& finalLigandList);
    };

    class OfflineDatabase 
    {
      public:
        OfflineDatabase() { this->path_of_input_file = "nopath"; this->import_json_file(path_of_input_file); };
        OfflineDatabase(std::string& path_to_input_file) {
          this->import_json_file ( path_to_input_file);
        };
        void import_json_file( std::string& path_to_input_file );
        ~OfflineDatabase() { };

        nlohmann::json return_imported_database() { return glytoucanglyconnectdatabase; };

      private:
        std::string path_of_input_file;
        nlohmann::json glytoucanglyconnectdatabase;

    };

  }
}

#endif