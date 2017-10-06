<?xml version="1.0" encoding="UTF-8"?>

<!--
	title: sbt customized equal parts report (specific kind of bom)
	desc.: xml 2 pdf
	
	time: 16.11.2009
-->

<xsl:stylesheet version="1.0" 
	xmlns:fo="http://www.w3.org/1999/XSL/Format" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:sbtrpt="http://mrtplm.com/pdm/xmlrep"
	xmlns:rptsup="http://mrtplm.com/pdm/xmlrep-support"
	xmlns:xalan="http://xml.apache.org/xalan"
	xmlns:js="ext1"
	extension-element-prefixes="js">

	<xsl:output indent="yes" method="xml" encoding="iso-8859-1"/>

	<xsl:key name="assembly" match="sbtrpt:Node" use="@OBID"/>

	<xalan:component prefix="js" elements="do_inc set_lvl add_obid_ prt add_obid_doc" functions="fixlex numInString str_end reverse adapt_str get_num get_lvl chk_proc_prt chk_proc_doc str fig str_1 getEqualPartNumber">
		<xalan:script lang="javascript">
			var num = 0;
			var obids_prt = "";
			var obids_doc = "";
			var lvl = 0;

      // Mantis 397
      function fixlex(str_refdes){
        var pad_refdes = "";
        var i = 0;
        var charcount = 0;
        var numcount = 0;
        
        if(str_refdes.length > 5 || str_refdes.length == 0 )
          {return str_refdes;}
        else{
          // count numerals and get leading characters
          for(i = 0; str_refdes.length > i; i++){
            if(!isNaN(str_refdes.charAt(i)))
              {numcount++;}
            if(isNaN(str_refdes.charAt(i))){
      				pad_refdes += str_refdes.charAt(i);
      				charcount++;
      			}	
          }
          
          // padd to 4 characters and return them
          if(numcount == 1)
            {pad_refdes = pad_refdes + "00" + str_refdes.charAt(charcount);}
          if(numcount == 2)
            {pad_refdes = pad_refdes + "0" + str_refdes.charAt(charcount) + str_refdes.charAt(charcount + 1);}
          if(numcount > 2)
            {return str_refdes;}
            
          return pad_refdes;    
        }
      }

			function do_inc() {
				num++;
				return null;
			}
			function get_num() {
				return num;
			}
			function set_lvl(val) {
				lvl = val;
				return null;
			}
			function get_lvl() {
				return lvl;
			}
			function add_obid_prt(obid) {
				obids_prt += obid + ";";
				return null;
			}
			function chk_proc_prt(obid) {
				var ret = 1;
				if (obids_prt.indexOf(obid) == -1) {
					ret = 0;
				}
				return ret;
			}		 
			function add_obid_doc(obid) {
				obids_doc += obid + ";";
				return null;
			}
			function chk_proc_doc(obid) {
				var ret = 1;
				if (obids_doc.indexOf(obid) == -1) {
					ret = 0;
				}
				return ret;
			}
			function get_obid_prt() {
				return obids_prt;
			}
			function get_obid_doc() {
				return obids_doc;
			}
			// CR920
			// getting the characters in the beginning of a string
			// if the first string is empty, the second is taken
			function str(str_fn, str_lst) {
				var ret = "";
				var i = 0;
				if (str_lst.length != 0){
					for (i = 0; str_lst.length > i; i++) {
						if (isNaN(str_lst.charAt(i))) {
							ret += str_lst.charAt(i);
						} else {
							return ret;
						}
					}
				} else {
					for (i = 0; str_fn.length > i; i++) {
						if (isNaN(str_fn.charAt(i))) {
							ret += str_fn.charAt(i);
						} else {
							return ret;
						}
					}
				}
				return ret;
			}
			// CR920
			// getting the numbers in the string
			function fig(fig_fn, fig_lst) {
				var ret = "";
				var i = 0;
				
				if (fig_lst.length != 0){
					for (i = 0; fig_lst.length > i; i++) {
						if (!isNaN(fig_lst.charAt(i))) {
							ret += fig_lst.charAt(i);
						}
					}
				} else {
					for (i = 0; fig_fn.length > i; i++) {
						if (!isNaN(fig_fn.charAt(i))) {
							ret += fig_fn.charAt(i);
						}
					}
				}
				return ret;
			}
			function str_1(val) {
				var ret = "1";
				if (isNaN(val.charAt(0))) {
					ret = val.charAt(0);
					}
				return ret;
			}
			// CR920
			// filtering of AC-numbers/BN-numbers/
			function rm_str(val) {
				var ret=val;
				var searchString="noStringFound";
				
				// get string to search in number
				searchStrings = new Array("AC", "BN", "PN");				
				for(i = 0; searchStrings.length > i; i++) {
					if (val.match(searchStrings[i])==searchStrings[i]){
						searchString = searchStrings[i];				
					} 
				}
				
				// check strings
				if(val.match(searchString)==searchString) {
					var ret = val.replace(searchString,"00");
					return ret;
				}
				else
				{
					if (isNaN(val.charAt(0))) {
						ret = val.split("Q")[1];
					}
				}
				return ret;
			}
			// CR920
			// adapts a string so that at least every X char is a zero width space
			function adapt_str(val) {
				var X = 3;
				var ret = "";
				var tmp_part = "";
				var tmp;
				var splitter = "&#8203;";
				var length = val.length;
				if (X+1 > length){
					return val;
				}
				for (var i = 0; length > i; i+=X){
						tmp = i+X;
						tmp_part = "";
						if (tmp > length)						
							tmp = length;
						tmp_part = val.substring(i,tmp);
						tmp_part = tmp_part.concat(splitter);
						ret = ret.concat(tmp_part);
					}
				return ret;
			} 
			// CR920
			// reverses a string
			function reverse(str){
				var ret = "";
				var i=str.length;
				i=i-1;
				for (var x = i; x >=0; x--) {
					ret += str.charAt(x);
				}
				return ret;		
			}
			// CR920
			// getting the characters in the end of a string
			// if the first string is empty, the second is taken
			function str_end(str_fn, str_lst) {
				var ret = "";
				var i = 0;
				if (str_lst.length != 0){
					for (i = str_lst.length; i > 0; i--) {
						if (isNaN(str_lst.charAt(i-1))||(str_lst.charAt(i-1)== ' ')) {
							ret += str_lst.charAt(i-1);
						} else {
							ret = reverse(ret);
							return ret;
						}
					}
				} else {
					for (i = str_fn.length; i > 0; i--) {
						if (isNaN(str_fn.charAt(i-1))) {
							ret += str_fn.charAt(i-1);
						} else {
							ret = reverse(ret);
							return ret;
						}
					}
				}
				return "";
			}
			// CR920
			// get the number of numbers inside the string
			function numInString(str_test){
				var counter = 0;
				for (var i = 0; str_test.length > i; i++){
					if (!isNaN(str_test.charAt(i))){
						counter++;
						}
				}
				return counter;
			}
			
			
			// acording to the specification ot SBT for equal part report
			// determining the equal part number
			function getEqualPartNumber(str_gt_config, str_partnumber, str_equalpartnumber){
				// return value which is the equal parts number
				var EPN = 'calcProblems';	
				var pn;
				var eqpn;		
							
				// information about var configuration
				// 0 ... A5Q
				// 1 ... 74/4
				// 2 ... others
					
				// get information about partnumber
				if(str_partnumber.charAt(0)=='A'){
					if(str_partnumber.charAt(1)=='5'){
					 if(str_partnumber.charAt(2)=='Q'){
						pn = 0;
					 } else if(str_partnumber.charAt(2)=='W'){
						EPN = str_partnumber;	
						return EPN;
					 } else {
						pn = 2;
					 }
					}
					else {
						pn = 2;
					}
				}
				else if((str_partnumber.charAt(0)=='7') || (str_partnumber.charAt(0)=='4')){
					pn = 1;
				}
				else {
					pn = 2;
				}
				
				// get information about equal part number
				if(str_equalpartnumber.charAt(0)=='A'){
					if(str_equalpartnumber.charAt(1)=='5'){
					 if(str_equalpartnumber.charAt(2)=='Q'){
						eqpn = 0;
					 } else {
						eqpn = 2;
					 }
					}
					else {
						eqpn = 2;
					}
				}
				else if( (str_equalpartnumber.charAt(0)=='7') || (str_equalpartnumber.charAt(0)=='4')){
					eqpn = 1;
				}
				else {
					eqpn = 2;
				}
				
				// the following algorithm is implicated by the specification of SBT
				// determining algorithm if l0GTRMaster is set to A5Q (l7_A5QNum)
				if (str_gt_config.charAt(3)=='A'){				
					if(pn == 0){
						if(eqpn == 0 || eqpn == 1 || eqpn == 2){
							EPN = str_partnumber;	
							return EPN;
						}
					}
					else if(pn == 1){
						if(eqpn == 0){
							EPN = str_equalpartnumber;
							return EPN;
						}
						else if(eqpn == 1 || eqpn == 2){
							EPN = str_partnumber;
							return EPN;
						}
					}
					else if(pn == 2){
						if(eqpn == 0){
							EPN = str_equalpartnumber;
							return EPN;
						}
						else if (eqpn == 1 || eqpn == 2){
							EPN = str_partnumber;
							return EPN;
						}
					}	
				}
				
			// determining algorithm if l0GTRMaster is set to 74../4.. (l7_74Num)
				else {
					if(pn == 0){
						if(eqpn == 0 || eqpn == 2){
							EPN = str_partnumber;
							return EPN;
						}
						if (eqpn == 1){
							EPN = str_equalpartnumber;
							return EPN;
						}
					}
				  else if (pn == 1){
				  	EPN = str_partnumber;
				  	return EPN;
				  }	
				  else if (pn == 2){
				  	if (eqpn == 0 || eqpn == 2){
				  		EPN = str_partnumber;
				  		return EPN;
				  	}
				  	if (eqpn == 1){
				  		EPN = str_equalpartnumber;
				  		return EPN;
				  	}
				  }
				}				
			}

		</xalan:script>
	</xalan:component>

	<!--	params  -->
	<xsl:param name="l0GTBRMaster"/>  <!-- l7_A5QNum, l7_74Num -->
	<xsl:param name="m0ProcessingLvl"/>
	<xsl:param name="m0MListStdCmpLV"/>
	<xsl:param name="m0ForceSingleLine"/> 
	<xsl:param name="l0ReportLocale"/> <!-- DE/EN -->
	<xsl:param name="l0SortForGTRep"/> <!-- 744 -->

<!--	identification strings -->
	<!-- CR920 - add l0ElcPro to asmb_str -->
	<xsl:variable name="asmb_str" select="'Assembly,DocuPart,j9Pkg,j9Tool,l0AsmPrg,l0AsSoft,l0ElcPro'"/>
	<xsl:variable name="comp_str" select="'Cmponent,l0SbtCmp,j9Aux,j9Mat'"/>
	<xsl:variable name="doc_str"  select="'dclass_dev,dclass_manu,dclass_UaD,dclass_MaP'"/>
	<xsl:variable name="auxr_str" select="'j9Pkg,j9Tool'"/>
	<xsl:variable name="auxm_str" select="'j9Aux,j9Mat'"/>
	
<!--    configuration strings    -->
<!--       *** CR920 ***         -->
	<!-- relation information (FindNumber, l0RefDesigListCl) will be displayed  -->
	<!-- (for objects with no value for "l0TechInfo" these attributs will not   -->
	<!--  be displayed automatically!!!                                         -->
	<xsl:variable name="add_info_relation" select="'l0AsmPrg,l0ElcPro,Cmponent,l0SbtCmp,Assembly'"/>
	<!-- configurationstring for relations between Assembly and Documents       -->
	<!-- (add relations class for displaying related Documents                  -->
	<xsl:variable name="ass2doc_relations" select="'l0DocPrt,PartDoc'"/>
	
	<!-- global var -->
	<xsl:variable name="type_display">
		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'DE'">
				<xsl:value-of select="'Gleichteile-Bauteile Report'"/>
			</xsl:when>
			<xsl:when test="$l0ReportLocale = 'EN'">
				<xsl:value-of select="'Equal Parts Bill of Material'"/>
			</xsl:when>
		</xsl:choose>
	</xsl:variable>
	
	<xsl:variable name="nr_display">
		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'DE'">
				<xsl:value-of select="'für ' + string(sbtrpt:Field[@Name='PartNumber'])"/>
			</xsl:when>
			<xsl:when test="$l0ReportLocale = 'EN'">
				<xsl:value-of select="'for ' + string(sbtrpt:Field[@Name='PartNumber'])"/>
			</xsl:when>
		</xsl:choose>
	</xsl:variable>

<!--	recursion controller -->
	<xsl:template name="rec_con">
		<xsl:param name="lvl"/>
		<xsl:if test="($lvl=0) and (count(sbtrpt:Relation/sbtrpt:Node[contains($doc_str, @Class)]) > 0)">
			<xsl:call-template name="doc_inc">
			  <!-- here we start with displaying the documents of the root assembly -->
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
		</xsl:if>		
		<!-- do the sorting for not SMDs -->
		<xsl:if test="not($l0SortForGTRep = 'l0SMDevice' or
											$l0SortForGTRep = 'l0RefDesigListCl' or
											$l0SortForGTRep = 'EqualPartNumber')">
			<xsl:for-each select="sbtrpt:Relation/sbtrpt:Node[contains($asmb_str, @Class) or contains($comp_str, @Class)]">
					<xsl:sort select="string(sbtrpt:Field[@Name=$l0SortForGTRep])" order="ascending"/>
				<xsl:choose>
					<xsl:when test="$lvl=0">
						<xsl:call-template name="part_inc">
							<xsl:with-param name="lvl" select="$lvl+1"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<xsl:call-template name="part_inc">
							<xsl:with-param name="lvl" select="$lvl"/>
						</xsl:call-template>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:for-each>
		</xsl:if>
		<!-- do the sorting for SMDs -->
		<xsl:if test="($l0SortForGTRep = 'l0SMDevice')">
			<xsl:for-each select="sbtrpt:Relation/sbtrpt:Node[contains($asmb_str, @Class) or contains($comp_str, @Class)]">
				<xsl:sort select="string(sbtrpt:Field[@Name=$l0SortForGTRep])" order="descending"/>
				<xsl:sort select="string(../sbtrpt:Field[@Name='l0RefDesigListCl'])" order="ascending"/>
				<xsl:choose>
					<xsl:when test="$lvl=0">
						<xsl:call-template name="part_inc">
							<xsl:with-param name="lvl" select="$lvl+1"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<xsl:call-template name="part_inc">
							<xsl:with-param name="lvl" select="$lvl"/>
						</xsl:call-template>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:for-each>
		</xsl:if>
		<!-- do the sorting for EqualPartNumber -->
		<xsl:if test="($l0SortForGTRep = 'EqualPartNumber')">
			<xsl:for-each select="sbtrpt:Relation/sbtrpt:Node[contains($asmb_str, @Class) or contains($comp_str, @Class)]">
				<xsl:sort select="string(js:getEqualPartNumber(string($l0GTBRMaster),string(sbtrpt:Field[@Name='PartNumber']),string(sbtrpt:Field[@Name='L0DocCode'])))" order="ascending"/>
				<xsl:choose>
					<xsl:when test="$lvl=0">
						<xsl:call-template name="part_inc">
							<xsl:with-param name="lvl" select="$lvl+1"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<xsl:call-template name="part_inc">
							<xsl:with-param name="lvl" select="$lvl"/>
						</xsl:call-template>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:for-each>
		</xsl:if>
		<!-- do the sorting for Reference Designator -->
		<xsl:if test="($l0SortForGTRep = 'l0RefDesigListCl')">
			<xsl:for-each select="sbtrpt:Relation/sbtrpt:Node[contains($asmb_str, @Class) or contains($comp_str, @Class)]">
				<xsl:sort select="js:fixlex(string(../sbtrpt:Field[@Name='l0RefDesigListCl']))" order="ascending"/>
				<xsl:choose>
					<xsl:when test="$lvl=0">
						<xsl:call-template name="part_inc">
							<xsl:with-param name="lvl" select="$lvl+1"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<xsl:call-template name="part_inc">
							<xsl:with-param name="lvl" select="$lvl"/>
						</xsl:call-template>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:for-each>
		</xsl:if>
	</xsl:template>

<!--	recursion processing configuration -->
	<xsl:template name="rec_conf">
		<xsl:param name="lvl"/>
	<!--	ruleset
				#1: processing level limit
				#2: concerned by rel 2 bom doc
				#3: l9ContinueStopBOMC set to '+' (Auxr.)
				#4: relation is l0AuRvRv (Auxm.)
				#5: item already processed
				#6: assembly with parttype = l7LGCodPEq
				#7: superseded = true
				#8: element is pcb
	-->
		<xsl:if test="not(($lvl = $m0ProcessingLvl) or
			(count(sbtrpt:Relation[@Class=$ass2doc_relations]/sbtrpt:Node[@Class='LGBomDoc']) > 0) or
			(contains($auxr_str, @Class)) or
			(contains($auxm_str, @Class)) or
			(js:chk_proc_prt(string(@OBID)) = 1) or
			(contains($asmb_str, @Class) and (sbtrpt:Field[@Name='PartType'] = 'l7LGCodPEq')) or
			(sbtrpt:Field[@Name='Superseded'] = 'True') or
			(contains(sbtrpt:Field[@Name='PartType'], 'PCB')))">
			<xsl:if test="count(sbtrpt:Relation/sbtrpt:Node[contains($asmb_str, @Class) or contains($comp_str, @Class)]) > 0">
				<xsl:value-of select="js:add_obid_prt(string(@OBID))"/>
			</xsl:if>
			<xsl:call-template name="rec_con">
				<xsl:with-param name="lvl" select="$lvl + 1"/>
			</xsl:call-template>
		</xsl:if>
	</xsl:template>

<!--	part inclusion -->
	<xsl:template name="part_inc">
	<!--	ruleset 
				#1: component is superseded
	-->
		<xsl:param name="lvl"/>
		<xsl:if test="not(sbtrpt:Field[@Name='Superseded'] = 'True')">
			<xsl:call-template name="rec_part">
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
		</xsl:if>
	</xsl:template>

<!--	document inclusion -->
	<xsl:template name="doc_inc">
		<xsl:param name="lvl"/>
	<!--	ruleset
			  ** @ top level  
				#1: include docs of buy components
				#2: l0DocInBom attribute in relation
				#3: revisions
				#4: relative part is superseded
				#5: item already processed
			  ** specifying docs
				#6: specifying doc and
				#6.1: relative part is a component and
				#6.2: standart part indicator is true
	-->
		<xsl:for-each select="sbtrpt:Relation/sbtrpt:Node[contains($doc_str, @Class)]">
			<xsl:sort select="js:str_1(string(sbtrpt:Field[@Name='DocumentName']))" order="ascending"/>
			<xsl:sort select="js:rm_str(string(sbtrpt:Field[@Name='DocumentName']))" order="ascending"/>
			<xsl:if test="not
						  (
						  	(($m0MListStdCmpLV = '-') and (../@Class = 'l0PrDcLv')) or 	
						  	(((../../@Class = 'Cmponent') or (../../@Class = 'l0SbtCmp')) and (../../sbtrpt:Field[@Name='PartType'] = 'Electronic Component') and not(../@Class = 'l0PrDcLv')) or
					      	(sbtrpt:Field[@Name='Superseded'] = 'True') or
						  	(js:chk_proc_doc(string(@OBID)) = 1 and 
						  		( not(contains('l0ElcPro',../../@Class)) and not(../@Class = 'l0PrDcLv')) ) or 
						  	((../@Class = 'PartDoc') and contains($comp_str, ../../@Class) and (../../sbtrpt:Field[@Name='StdPartIndicator'] = '+'))
						  )">
				<xsl:value-of select="js:add_obid_doc(string(@OBID))"/>
				<xsl:call-template name="rec_doc">
					<xsl:with-param name="lvl" select="$lvl"/>
				</xsl:call-template>
			</xsl:if>
		</xsl:for-each>
	</xsl:template>
	
	<xsl:template match="sbtrpt:ResultSet">
		<fo:root xmlns:fo="http://www.w3.org/1999/XSL/Format" line-height="1">
			<fo:layout-master-set>
				<fo:simple-page-master master-name="a4" page-height="29.7cm" page-width="21cm" margin-top="1cm" margin-bottom="1cm" margin-left="1cm" margin-right="1cm">
					<fo:region-body margin-bottom="3.5cm"/>
					<fo:region-after extent="3cm"/>
				</fo:simple-page-master>
			</fo:layout-master-set>
			<xsl:apply-templates/>
		</fo:root>
	</xsl:template>

	<!-- making the geräte-software bom available by adding l0AsSoft here -->
	<!-- remark for further expansion: @Class='LGProdkt' or @Class='Assembly' or @Class='l0AsSoft' or @Class='j9Pkg' -->
	<xsl:template match="sbtrpt:ResultSet/sbtrpt:Node[@Class='Assembly']">
		<fo:page-sequence master-reference="a4" force-page-count="no-force" initial-page-number="1">
			<fo:static-content flow-name="xsl-region-after">
				<xsl:call-template name="footer">
					<xsl:with-param name="nid" select="@OBID"/>
				</xsl:call-template>
			</fo:static-content>
			<fo:flow flow-name="xsl-region-body">
				<xsl:call-template name="body"/>
				<fo:block id="end"/>
			</fo:flow>
		</fo:page-sequence>
	</xsl:template>
		
	<!--	Start here with the footer of the report -->
	<!--	Created:
			  Released:
			  Responsible Person:
			  Logo / Index / Page
	-->
	<xsl:template name="footer">
		<fo:table table-layout="fixed" width="19cm">
			<fo:table-column column-width="9cm"/>
			<fo:table-column column-width="7cm"/>
			<fo:table-column column-width="3cm"/>
			<fo:table-body font-size="8pt">
				<fo:table-row>
					<fo:table-cell display-align="center" text-align="start" padding-left="2pt" padding-top="2pt"  border-left="1pt solid black" border-top="1pt solid black" border-bottom="0.5pt solid black">
						<xsl:variable name="naming_credate">
								<xsl:choose>
									<xsl:when test="$l0ReportLocale = 'DE'">
										<xsl:value-of select="'Erstellt: '"/>
									</xsl:when>
									<xsl:when test="$l0ReportLocale = 'EN'">
										<xsl:value-of select="'Creation: '"/>
									</xsl:when>
								</xsl:choose>
							</xsl:variable>
						<fo:block>
							<fo:inline font-weight="bold">
								<xsl:value-of select="$naming_credate"/>
							</fo:inline>
							<xsl:value-of select="../@Date"/>
							<xsl:text> @ </xsl:text>
							<xsl:value-of select="../@Time"/>
							<xsl:text>, </xsl:text>
							<xsl:value-of select="../@Creator"/>
						</fo:block>
					</fo:table-cell>
					<fo:table-cell display-align="center" text-align="end" number-columns-spanned="2" padding-right="2pt" padding-top="2pt" border-top="1pt solid black" border-bottom="0.5pt solid black" border-right="1pt solid black">
						<fo:block font-size="10pt">
						</fo:block>
					</fo:table-cell>
				</fo:table-row>
				<fo:table-row>
					<fo:table-cell display-align="center" text-align="start" padding-left="2pt" padding-top="2pt"  border-left="1pt solid black" border-bottom="0.5pt solid black">
						<xsl:variable name="naming_reldate">
								<xsl:choose>
									<xsl:when test="$l0ReportLocale = 'DE'">
										<xsl:value-of select="'Freig.:'"/>
									</xsl:when>
									<xsl:when test="$l0ReportLocale = 'EN'">
										<xsl:value-of select="'Release: '"/>
									</xsl:when>
								</xsl:choose>
							</xsl:variable>
						<fo:block>
							<fo:inline font-weight="bold">
								<xsl:value-of select="$naming_reldate"/>
							</fo:inline>
						</fo:block>
					</fo:table-cell>
					<fo:table-cell display-align="center" text-align="end" number-columns-spanned="2" padding-right="2pt" padding-top="2pt" border-bottom="0.5pt solid black" border-right="1pt solid black">
						<fo:block font-weight="bold" font-size="10pt">
							<xsl:value-of select="sbtrpt:Field[@Name='Nomenclature']"/>
						</fo:block>
					</fo:table-cell>
				</fo:table-row>
				<fo:table-row>
					<fo:table-cell display-align="center" number-columns-spanned="2" text-align="start" padding-left="2pt" padding-top="2pt"  border-left="1pt solid black" border-bottom="0.5pt solid black">
						<fo:block>
							<xsl:variable name="naming_respperson">
								<xsl:choose>
									<xsl:when test="$l0ReportLocale = 'DE'">
										<xsl:value-of select="'Verantw. Stelle: '"/>
									</xsl:when>
									<xsl:when test="$l0ReportLocale = 'EN'">
										<xsl:value-of select="'Resp. Person: '"/>
									</xsl:when>
								</xsl:choose>
							</xsl:variable>
							<fo:inline font-weight="bold">
								<xsl:value-of select="$naming_respperson"/>
							</fo:inline>
							<xsl:value-of select="sbtrpt:Field[@Name='ResponsablePerson']"/>
							<xsl:text>, </xsl:text>
							<xsl:value-of select="sbtrpt:Field[@Name='LGRespPhone']"/>
							<xsl:text>, </xsl:text>
							<xsl:value-of select="sbtrpt:Field[@Name='LGRespUnit']"/>
						</fo:block>
					</fo:table-cell>
					<fo:table-cell display-align="center" text-align="end" padding-right="2pt" padding-top="2pt" border-bottom="0.5pt solid black" border-right="1pt solid black">
						<fo:block>
							<fo:inline font-weight="bold">
								<xsl:text>Index: </xsl:text>
							</fo:inline>
							<xsl:value-of select="sbtrpt:Field[@Name='Revision']"/>
						</fo:block>
					</fo:table-cell>
				</fo:table-row>
				<fo:table-row>
					<fo:table-cell display-align="center" text-align="center" number-rows-spanned="2" padding-left="2pt" padding-top="2pt" border-bottom="1pt solid black" border-left="1pt solid black">
						<fo:block>
							<fo:external-graphic content-height="0.6cm" height="0.6cm" src="siemens.jpg"/>
						</fo:block>
						<fo:block font-size="7pt">
							<xsl:text>Copyright 1996 by Siemens Building Technologies AG</xsl:text>
						</fo:block>
					</fo:table-cell>
					<fo:table-cell text-align="center" padding-left="2pt" padding-top="2pt" border-left="0.5pt solid black" border-right="0.5pt solid black">
						<fo:block font-weight="bold"  font-size="11pt">
							<xsl:value-of select="$type_display"/>
								<xsl:choose>
									<xsl:when test="$l0ReportLocale = 'DE'">
										<xsl:text> für: </xsl:text>
									</xsl:when>
									<xsl:when test="$l0ReportLocale = 'EN'">
										<xsl:text> for: </xsl:text>
									</xsl:when>
								</xsl:choose>
						</fo:block>
						<fo:block font-size="7pt">
						  <xsl:choose>
		            <xsl:when test="$l0ReportLocale='DE'">
                  <xsl:value-of select="' Gleichteilnummer: '"/>
				      		  <xsl:if test="$l0GTBRMaster = 'l7_A5QNum'">
				      		    <xsl:value-of select="'A5Q...'"/>
				      		  </xsl:if>
				      		  <xsl:if test="$l0GTBRMaster = 'l7_74Num'">
				      		    <xsl:value-of select="'74.../4...'"/>
				      		  </xsl:if>
                  <xsl:value-of select="' Sortiert nach: '"/>
                    <xsl:if test="not($l0SortForGTRep = 'l0RefDesigListCl' or
				      		  					        $l0SortForGTRep = 'EqualPartNumber' or
				      		  					        $l0SortForGTRep = 'l0SMDevice')">
				      		    <xsl:value-of select="'Identnummer'"/>
				      		  </xsl:if>
				      		  <xsl:if test="$l0SortForGTRep = 'l0RefDesigListCl'">
				      		    <xsl:value-of select="'Referenzdesignator'"/>
				      		  </xsl:if>
				      		  <xsl:if test="$l0SortForGTRep = 'l0SMDevice'">
				      		    <xsl:value-of select="'SMD'"/>
				      		  </xsl:if>
				      		  <xsl:if test="$l0SortForGTRep = 'EqualPartNumber'">
				      		    <xsl:value-of select="'Gleichteilnummer'"/>
				      		  </xsl:if>  
                </xsl:when>
                <xsl:when test="$l0ReportLocale = 'EN'">
                  <xsl:value-of select="'EqualPartNumber: '"/>
				      		  <xsl:if test="$l0GTBRMaster = 'l7_A5QNum'">
				      		    <xsl:value-of select="'A5Q...'"/>
				      		  </xsl:if>
				      		  <xsl:if test="$l0GTBRMaster = 'l7_74Num'">
				      		    <xsl:value-of select="'74.../4...'"/>
				      		  </xsl:if>
				      		<xsl:value-of select="' Sorted by: '"/>
                    <xsl:if test="not($l0SortForGTRep = 'l0RefDesigListCl' or
				      		  					        $l0SortForGTRep = 'EqualPartNumber' or
				      		  					        $l0SortForGTRep = 'l0SMDevice')">
				      		    <xsl:value-of select="'Identnumber'"/>
				      		  </xsl:if>
				      		  <xsl:if test="$l0SortForGTRep = 'l0RefDesigListCl'">
				      		    <xsl:value-of select="'reference designator'"/>
				      		  </xsl:if>
				      		  <xsl:if test="$l0SortForGTRep = 'l0SMDevice'">
				      		    <xsl:value-of select="'SMD'"/>
				      		  </xsl:if>
				      		  <xsl:if test="$l0SortForGTRep = 'EqualPartNumber'">
				      		    <xsl:value-of select="'EqualPartNumber'"/>
				      		  </xsl:if>
				      		  
                </xsl:when>
              </xsl:choose>
						</fo:block>
					</fo:table-cell>
					<fo:table-cell padding-left="2pt" padding-top="2pt" border-right="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:variable name="naming_sheet">
								<xsl:choose>
									<xsl:when test="$l0ReportLocale = 'DE'">
										<xsl:value-of select="'Blatt'"/>
									</xsl:when>
									<xsl:when test="$l0ReportLocale = 'EN'">
										<xsl:value-of select="'Sheet'"/>
									</xsl:when>
								</xsl:choose>
							</xsl:variable>
							<xsl:value-of select="$naming_sheet"/>
						</fo:block>
					</fo:table-cell>
				</fo:table-row>
				<fo:table-row height="1.2cm">
					<fo:table-cell text-align="center" padding-left="2pt" padding-top="20pt" border-bottom="1pt solid black" border-left="0.5pt solid black" border-right="0.5pt solid black">
						<fo:block font-weight="bold" font-size="10pt" >
							<xsl:value-of select="js:getEqualPartNumber(string($l0GTBRMaster),string(sbtrpt:Field[@Name='PartNumber']),string(sbtrpt:Field[@Name='L0DocCode']))"/>
						</fo:block>
					</fo:table-cell>
					<fo:table-cell text-align="center" padding-left="2pt" padding-top="20pt" border-bottom="1pt solid black" border-right="1pt solid black">
						<fo:block font-weight="bold" font-size="10pt">
							<fo:page-number/>
							<xsl:text> / </xsl:text>
							<fo:page-number-citation ref-id="end"/>
						</fo:block>
					</fo:table-cell>
				</fo:table-row>
			</fo:table-body>
		</fo:table>
	</xsl:template>

	<xsl:template name="body">
		<fo:table table-layout="fixed" width="19cm">
			<fo:table-column column-width="0.8cm"/>
			<fo:table-column column-width="0.8cm"/>
			<fo:table-column column-width="0.8cm"/>
			<fo:table-column column-width="0.8cm"/>
			<fo:table-column column-width="2.2cm"/>
			<fo:table-column column-width="2.2cm"/>
			<fo:table-column column-width="4.2cm"/>
			<fo:table-column column-width="4.2cm"/>
			<fo:table-column column-width="0.8cm"/>
			<fo:table-column column-width="1cm"/>
			<fo:table-column column-width="1.2cm"/>
			<fo:table-header>
				<fo:table-row font-size="8pt">
					
					<fo:table-cell display-align="center" padding-top="2pt" padding-left="2pt" border-bottom="0.5pt solid black" border-left="1pt solid black" border-top="1pt solid black">					
						<fo:block font-weight="bold">
							<xsl:text>#</xsl:text>
						</fo:block>
					</fo:table-cell>
					
					<xsl:variable name="naming_qty">
						<xsl:choose>
							<xsl:when test="$l0ReportLocale = 'DE'">
								<xsl:value-of select="'Anz'"/>
							</xsl:when>
							<xsl:when test="$l0ReportLocale = 'EN'">
								<xsl:value-of select="'Qty'"/>
							</xsl:when>
						</xsl:choose>
					</xsl:variable>
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
								<xsl:value-of select="$naming_qty" />
						</fo:block>
					</fo:table-cell>
					
					<xsl:variable name="naming_unit">
						<xsl:choose>
							<xsl:when test="$l0ReportLocale = 'DE'">
								<xsl:value-of select="'Einh'"/>
							</xsl:when>
							<xsl:when test="$l0ReportLocale = 'EN'">
								<xsl:value-of select="'Un'"/>
							</xsl:when>
						</xsl:choose>
					</xsl:variable>
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:value-of select="$naming_unit" />
						</fo:block>
					</fo:table-cell>
					
					<xsl:variable name="naming_level">
						<xsl:choose>
							<xsl:when test="$l0ReportLocale = 'DE'">
								<xsl:value-of select="'Stufe'"/>
							</xsl:when>
							<xsl:when test="$l0ReportLocale = 'EN'">
								<xsl:value-of select="'Lvl'"/>
							</xsl:when>
						</xsl:choose>
					</xsl:variable>
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:value-of select="$naming_level" />
						</fo:block>
					</fo:table-cell>
					
					<xsl:variable name="naming_ident">
						<xsl:choose>
							<xsl:when test="$l0ReportLocale = 'DE'">
								<xsl:value-of select="'Ident'"/>
							</xsl:when>
							<xsl:when test="$l0ReportLocale = 'EN'">
								<xsl:value-of select="'Ident'"/>
							</xsl:when>
						</xsl:choose>
					</xsl:variable>
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							 <xsl:value-of select="$naming_ident" />
						</fo:block>
					</fo:table-cell>
					
					<!-- <fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:text>Rev.</xsl:text>
						</fo:block>
					</fo:table-cell>
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:text>Stat.</xsl:text>
						</fo:block>
					</fo:table-cell> -->
					
					<xsl:variable name="naming_ep_number">
						<xsl:choose>
							<xsl:when test="$l0ReportLocale = 'DE'">
								<xsl:value-of select="'Gl.teil-Nr.'"/>
							</xsl:when>
							<xsl:when test="$l0ReportLocale = 'EN'">
								<xsl:value-of select="'Eq.part No.'"/>
							</xsl:when>
						</xsl:choose>
					</xsl:variable>
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:value-of select="$naming_ep_number" />
						</fo:block>
					</fo:table-cell>
					
					<xsl:variable name="naming_nomenclature">
						<xsl:choose>
							<xsl:when test="$l0ReportLocale = 'DE'">
								<xsl:value-of select="'Benennung'"/>
							</xsl:when>
							<xsl:when test="$l0ReportLocale = 'EN'">
								<xsl:value-of select="'Nomenclature'"/>
							</xsl:when>
						</xsl:choose>
					</xsl:variable>
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:value-of select="$naming_nomenclature" />
						</fo:block>
					</fo:table-cell>
					
					<xsl:variable name="naming_techinfo">
						<xsl:choose>
							<xsl:when test="$l0ReportLocale = 'DE'">
								<xsl:value-of select="'Tech. Info'"/>
							</xsl:when>
							<xsl:when test="$l0ReportLocale = 'EN'">
								<xsl:value-of select="'Tech. Info'"/>
							</xsl:when>
						</xsl:choose>
					</xsl:variable>
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:value-of select="$naming_techinfo" />
						</fo:block>
					</fo:table-cell>
					
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:text>SMD</xsl:text>
						</fo:block>
					</fo:table-cell>
					
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:text>F/N</xsl:text>
						</fo:block>
					</fo:table-cell>
					
					<fo:table-cell display-align="center" padding-top="2pt" border-bottom="0.5pt solid black" border-right="1pt solid black" border-top="1pt solid black">
						<fo:block font-weight="bold">
							<xsl:text>Ref.</xsl:text>
						</fo:block>
					</fo:table-cell>
					
				</fo:table-row>
			</fo:table-header>
			<fo:table-footer>
				<fo:table-row>
					<fo:table-cell display-align="center" padding-top="2pt" border-top="1pt solid black">
						<xsl:attribute name="number-columns-spanned">11</xsl:attribute>
						<fo:block><xsl:text> </xsl:text></fo:block>
					</fo:table-cell>
				</fo:table-row>
			</fo:table-footer>
			<xsl:choose>
    			<xsl:when test="count(//sbtrpt:Relation) > 0">
              <fo:table-body font-size="8pt">
        				<xsl:call-template name="rec_con">
        					<xsl:with-param name="lvl" select="0"/>
        				</xsl:call-template>
    			    </fo:table-body>
    			</xsl:when>
    			<xsl:otherwise>
              <fo:table-body font-size="8pt">
        				<fo:table-row>
        					<fo:table-cell display-align="center" padding-top="2pt" border-top="1pt solid black">
        						<xsl:attribute name="number-columns-spanned">11</xsl:attribute>
        							<fo:block><xsl:text> </xsl:text></fo:block>
        					</fo:table-cell>
        				</fo:table-row>
    			    </fo:table-body>
    			</xsl:otherwise>
      </xsl:choose>
		</fo:table>
	</xsl:template>

	<xsl:template name="cell_val">
		<xsl:param name="val"/>
		<xsl:param name="lvl"/>
		<xsl:param name="b_l"/>
		<xsl:param name="b_r"/>
		<xsl:param name="n_c_s"/>
		<xsl:param name="ref_oc"/>
		<fo:table-cell padding-top="2pt" padding-left="2pt" padding-right="2pt">
			<xsl:if test="$n_c_s > 0">
				<xsl:attribute name="number-columns-spanned">
					<xsl:value-of select="$n_c_s"/>
				</xsl:attribute>
			</xsl:if>
			<xsl:if test="not(js:get_lvl() = $lvl)">
				<xsl:choose>
					<xsl:when test="$lvl = 1">
						<xsl:attribute name="border-top">0.5pt solid black</xsl:attribute>
					</xsl:when>
					<xsl:otherwise>
						<xsl:attribute name="border-top">0.5pt dashed black</xsl:attribute>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:if>
			<xsl:if test="($m0ForceSingleLine = '+') and not($ref_oc = 'True')">
				<xsl:attribute name="wrap-option">no-wrap</xsl:attribute>
				<xsl:attribute name="display-align">center</xsl:attribute>
			</xsl:if>
			<xsl:if test="js:get_num() mod 2">
				<xsl:attribute name="background-color">#E8E8E8</xsl:attribute>
			</xsl:if>
			<xsl:if test="$b_l">
				<xsl:attribute name="border-left">1pt solid black</xsl:attribute>
			</xsl:if>
			<xsl:if test="$b_r">
				<xsl:attribute name="border-right">1pt solid black</xsl:attribute>
			</xsl:if>
			<fo:block-container>
				<xsl:if test="$m0ForceSingleLine = '+'">
					<xsl:attribute name="overflow">hidden</xsl:attribute>
				</xsl:if>
				<fo:block hyphenate="true">
					<xsl:value-of select="$val"/>
				</fo:block>
			</fo:block-container>
		</fo:table-cell>
	</xsl:template>

<!--	recursion: part node -->
	<xsl:template name="rec_part">
		<xsl:param name="lvl"/>
		<!-- get value: # -->
		<xsl:variable name="s_l_o">
			<xsl:if test="contains(../sbtrpt:Field[@Name='l0RefDesigListCl'],',') 
						  or contains(../sbtrpt:Field[@Name='l0RefDesigListCl'],' ')
						  or contains(../sbtrpt:Field[@Name='FindNumber'],',')
						  or contains(../sbtrpt:Field[@Name='FindNumber'],' ')">
				<xsl:value-of select="'True'"/>
			</xsl:if>
		</xsl:variable>
		<!-- increase value: # -->
		<js:do_inc/>
		<fo:table-row>
			<!-- display value: # -->
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="js:get_num()"/>
				<xsl:with-param name="b_l" select="1"/>
				<xsl:with-param name="lvl" select="$lvl"/>
				<xsl:with-param name="ref_oc" select="$s_l_o"/>
			</xsl:call-template>
			<!-- get value: Qty -->
			<xsl:variable name="qty_v">
				<xsl:choose>
					<xsl:when test="../sbtrpt:Field[@Name='Quantity'] = 'l0req'">
						<xsl:value-of select="'req'"/>
					</xsl:when>
					<xsl:when test="../sbtrpt:Field[@Name='Quantity'] = 'l0Kar'">
						<xsl:value-of select="'Box'"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="../sbtrpt:Field[@Name='Quantity']"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<!-- display value: Qty -->
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="$qty_v"/>
				<xsl:with-param name="lvl" select="$lvl"/>
				<xsl:with-param name="ref_oc" select="$s_l_o"/>
			</xsl:call-template>
			<!-- get value: Un and transform to other format is necessary -->
			<xsl:variable name="un_v">
				<xsl:choose>
					<!-- display english units -->
					<xsl:when test="$l0ReportLocale = 'EN'">			
						<xsl:choose>
							<xsl:when test="../sbtrpt:Field[@Name='UnitOfMeasure'] = 'ea'">
								<xsl:value-of select="'ea'"/>
							</xsl:when>
							<xsl:otherwise>
								<xsl:value-of select="../sbtrpt:Field[@Name='UnitOfMeasure']"/>
							</xsl:otherwise>
						</xsl:choose>
					</xsl:when>
					<!-- display german units -->
					<xsl:when test="$l0ReportLocale = 'DE'">
						<xsl:choose>
							<xsl:when test="../sbtrpt:Field[@Name='UnitOfMeasure']= 'ea'">
								<xsl:value-of select="'ea'"/>
							</xsl:when>
							<xsl:otherwise>
								<xsl:value-of select="../sbtrpt:Field[@Name='UnitOfMeasure']"/>
							</xsl:otherwise>
						</xsl:choose>
					</xsl:when>					
				</xsl:choose>
			</xsl:variable>
			<!-- display value: Un -->
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="$un_v"/>
				<xsl:with-param name="lvl" select="$lvl"/>
				<xsl:with-param name="ref_oc" select="$s_l_o"/>
			</xsl:call-template>
			<!-- get value: Lvl -->			
			<xsl:variable name="lvl_v">
				<xsl:choose>
					<xsl:when test="contains($auxm_str, @Class)">
						<xsl:value-of select="'M'"/>
					</xsl:when>
					<xsl:when test="$lvl > 1">
						<xsl:value-of select="concat('_',$lvl)"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$lvl"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<!-- display value: Lvl -->
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="$lvl_v"/>
				<xsl:with-param name="lvl" select="$lvl"/>
				<xsl:with-param name="ref_oc" select="$s_l_o"/>
			</xsl:call-template>
			<!-- get value: Ident. -->
			<xsl:variable name="code_v">
				<xsl:value-of select="sbtrpt:Field[@Name='PartNumber']"/>
			</xsl:variable>
			<!-- display value: Ident. -->
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="$code_v"/>
				<xsl:with-param name="lvl" select="$lvl"/>
				<xsl:with-param name="ref_oc" select="$s_l_o"/>
			</xsl:call-template>
			<!-- get value: Rev.
			<xsl:variable name="revision">
				<xsl:if test="$m0BomType = 'LcsWorking'">
					<xsl:value-of select="sbtrpt:Field[@Name='Revision']"/>
				</xsl:if>
			</xsl:variable> -->
			<!-- display value: Rev.
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="$revision"/>
				<xsl:with-param name="lvl" select="$lvl"/>
				<xsl:with-param name="ref_oc" select="$s_l_o"/>
			</xsl:call-template> -->
			<!-- get value: Stat.
			<xsl:variable name="type_v">
				<xsl:if test="$m0BomType = 'LcsWorking'">
					<xsl:choose>
						<xsl:when test="string-length(sbtrpt:Field[@Name='LifeCycleState']) > 5">
							<xsl:value-of select="concat(substring(sbtrpt:Field[@Name='LifeCycleState'],1,4),'.')"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="sbtrpt:Field[@Name='LifeCycleState']"/>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:if>
			</xsl:variable>  -->
			<!-- display value: Stat.
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="$type_v"/>
				<xsl:with-param name="lvl" select="$lvl"/>
				<xsl:with-param name="ref_oc" select="$s_l_o"/>
			</xsl:call-template>   -->
			
			<!-- get value: GT-Number -->
			<xsl:variable name="gt_number">
				<xsl:value-of select="js:getEqualPartNumber(string($l0GTBRMaster),string($code_v),string(sbtrpt:Field[@Name='L0DocCode']))"/>
			</xsl:variable> 
			<!-- display value: GT_Number -->
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="$gt_number"/>
				<xsl:with-param name="lvl" select="$lvl"/>
				<xsl:with-param name="ref_oc" select="$s_l_o"/>
			</xsl:call-template> 
			
			<xsl:choose>
				<!-- *** CR920 ***
        			 IF Class is $add_info_relation display the value for l0RefDesigListCl  
        			 (contains($add_info_relation, @Class))
        			-->
        		<xsl:when test="string-length(sbtrpt:Field[@Name='l0TechInfo']) = 0 and
        					    contains($add_info_relation, @Class)">
        			<xsl:call-template name="cell_val">
        				<xsl:with-param name="val" select="sbtrpt:Field[@Name='Nomenclature']"/>
        				<xsl:with-param name="lvl" select="$lvl"/>
        				<xsl:with-param name="ref_oc" select="$s_l_o"/>
        				<xsl:with-param name="b_r" select="0"/>
        				<xsl:with-param name="n_c_s" select="2"/>
        			</xsl:call-template>
        			<!-- get value: SMD -->
        			<xsl:variable name="smd_v">
								<xsl:if test="sbtrpt:Field[@Name='l0SMDevice'] = 'True'">
        					<xsl:value-of select="'J'"/>
        				</xsl:if>
							</xsl:variable>
							<!-- get value: SMD -->
							<xsl:call-template name="cell_val">
        				<xsl:with-param name="val" select="$smd_v"/>
        				<xsl:with-param name="lvl" select="$lvl"/>
        				<xsl:with-param name="ref_oc" select="$s_l_o"/>
        				<xsl:with-param name="b_r" select="0"/>
        				<xsl:with-param name="n_c_s" select="1"/>
        			</xsl:call-template>
        			<!-- get value: F/N -->
        			<xsl:variable name="fn_v">
								<xsl:choose>
									<xsl:when test="../sbtrpt:Field[@Name='FindNumber'] != 0">
										<xsl:value-of select="../sbtrpt:Field[@Name='FindNumber']"/>
									</xsl:when>
									<xsl:otherwise>
										<xsl:value-of select="../sbtrpt:Field[@Name='l0RefDesigListCl']"/>
									</xsl:otherwise>
								</xsl:choose>
							</xsl:variable>
	        		<!-- display value: F/N -->
	        		<xsl:call-template name="cell_val">
	        			<xsl:with-param name="val" select="js:adapt_str(string($fn_v))"/>
	        			<xsl:with-param name="b_r" select="0"/>
	        			<xsl:with-param name="lvl" select="$lvl"/>
	        			<xsl:with-param name="n_c_s" select="1"/>
	        			<xsl:with-param name="ref_oc" select="$s_l_o"/>
	        		</xsl:call-template>
        			<!-- get value: Ref. -->       			
	        		<xsl:variable name="ref_val">
	        			<xsl:choose>	
							<xsl:when test="(string-length(../sbtrpt:Field[@Name='l0RefDesigListCl']) > 5) or (../sbtrpt:Field[@Name='l0RefDesigListCl']='---')">
								<xsl:value-of select="../sbtrpt:Field[@Name='l0RefDesigListCl']"/>
							</xsl:when>
								<xsl:otherwise>
									<xsl:if test="(string-length(../sbtrpt:Field[@Name='l0RefDesigListCl']) > 0)">
										<xsl:value-of select="concat(js:str(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl'])), format-number(js:fig(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl'])), '000'), js:str_end(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl'])))"/>
									</xsl:if>
								</xsl:otherwise>
							</xsl:choose>
	        		</xsl:variable>
	        		<!-- display value: Ref. -->
	        		<xsl:call-template name="cell_val">
	        			<xsl:with-param name="val" select="js:adapt_str(string($ref_val))"/>
	        			<xsl:with-param name="b_r" select="1"/>
	        			<xsl:with-param name="lvl" select="$lvl"/>
	        			<xsl:with-param name="n_c_s" select="1"/>
	        			<xsl:with-param name="ref_oc" select="$s_l_o"/>
        			</xsl:call-template>
        		 </xsl:when>
				<!-- IF    attribute l0TechInfo lenght is 0 
					 THEN  attribute Nomenclature 	   is the last displayed attribute in this line
				-->
			    <xsl:when test="string-length(sbtrpt:Field[@Name='l0TechInfo']) = 0">
        			<xsl:call-template name="cell_val">
        				<!-- xsl:with-param name="val" select="sbtrpt:Field[@Name='Nomenclature']"/-->
        				<xsl:with-param name="val" select="js:adapt_str(string(sbtrpt:Field[@Name='Nomenclature']))"/>
        				<xsl:with-param name="lvl" select="$lvl"/>
        				<xsl:with-param name="ref_oc" select="$s_l_o"/>
        				<xsl:with-param name="b_r" select="0"/>
        				<xsl:with-param name="n_c_s" select="2"/>
        			</xsl:call-template>
        			<xsl:call-template name="cell_val">
        				<xsl:with-param name="val" select="' '"/>
        				<xsl:with-param name="lvl" select="$lvl"/>
        				<xsl:with-param name="ref_oc" select="$s_l_o"/>
        				<xsl:with-param name="b_r" select="1"/>
        				<xsl:with-param name="n_c_s" select="3"/>
        			</xsl:call-template>
        		</xsl:when>
			    <!-- ELSE  get and display all attributes -->
			    <xsl:otherwise>
			    	<!-- get and display value: Nomenclature -->
        			<xsl:call-template name="cell_val">
        				<!-- *** CR920 *** -->
        				<xsl:with-param name="val" select="js:adapt_str(string(sbtrpt:Field[@Name='Nomenclature']))"/>
        				<xsl:with-param name="lvl" select="$lvl"/>
        				<xsl:with-param name="ref_oc" select="$s_l_o"/>
        			</xsl:call-template>
        			<!-- get and dispaly value: l0TechInfo-->
        			<xsl:call-template name="cell_val">
        				<!-- *** CR920 *** -->
        				<xsl:with-param name="val" select="js:adapt_str(string(sbtrpt:Field[@Name='l0TechInfo']))"/>
        				<xsl:with-param name="lvl" select="$lvl"/>
        				<xsl:with-param name="ref_oc" select="$s_l_o"/>
        			</xsl:call-template>
        			<!-- get value: SMD -->
        			<xsl:variable name="smd_v">
        				<xsl:if test="sbtrpt:Field[@Name='l0SMDevice'] = 'True'">
        					<xsl:value-of select="'J'"/>
        				</xsl:if>	
        			</xsl:variable>
        			<!-- display value: SMD -->
        			<xsl:call-template name="cell_val">
        				<xsl:with-param name="val" select="$smd_v"/>
        				<xsl:with-param name="lvl" select="$lvl"/>
        				<xsl:with-param name="ref_oc" select="$s_l_o"/>
        			</xsl:call-template>
        			<!-- get value: F/N  -->
        			<xsl:variable name="fn_v">
        				<xsl:choose>
							<xsl:when test="../sbtrpt:Field[@Name='FindNumber'] != 0">
								<xsl:value-of select="js:adapt_str(string(../sbtrpt:Field[@Name='FindNumber']))"/>
							</xsl:when>
							<xsl:otherwise>
								<xsl:value-of select="js:adapt_str(string(../sbtrpt:Field[@Name='l0RefDesigListCl']))"/>
							</xsl:otherwise>
						</xsl:choose>
        			</xsl:variable>
					<!-- display value: F/N -->
        			<xsl:call-template name="cell_val">
        				<xsl:with-param name="val" select="$fn_v"/>
        				<xsl:with-param name="lvl" select="$lvl"/>
        				<xsl:with-param name="ref_oc" select="$s_l_o"/>
        			</xsl:call-template>
					<!-- get value: Ref. -->
        			<xsl:variable name="ref_v">  	
	        			<xsl:choose>	
	        				<xsl:when test="(string-length(../sbtrpt:Field[@Name='l0RefDesigListCl']) > 5) or (../sbtrpt:Field[@Name='l0RefDesigListCl']='---')">
								<xsl:value-of select="../sbtrpt:Field[@Name='l0RefDesigListCl']"/>
							</xsl:when>
							<xsl:otherwise>
								<xsl:if test="(string-length(../sbtrpt:Field[@Name='l0RefDesigListCl']) > 0)">
									<xsl:choose>
										<xsl:when test="js:numInString(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl'])) > 0">
											<xsl:value-of select="concat(js:str(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl'])), format-number(js:fig(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl'])), '000'), js:str_end(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl'])))"/>
										</xsl:when>
										<xsl:otherwise>
											<xsl:value-of select="concat(js:str(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl'])), js:str_end(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl'])))"/>
										</xsl:otherwise>
									</xsl:choose>
								</xsl:if>
							</xsl:otherwise>
						</xsl:choose>
	        		</xsl:variable>
        			<!-- display value: Ref. -->
        			<xsl:choose>
        				<!-- IF s_l_o (Single Line On) is true -->
        				<xsl:when test="$s_l_o = 'True'">
        					<xsl:call-template name="cell_val">
        						<xsl:with-param name="val" select="js:adapt_str(string(../sbtrpt:Field[@Name='l0RefDesigListCl']))"/>
        						<xsl:with-param name="b_r" select="1"/>
        						<xsl:with-param name="lvl" select="$lvl"/>
        						<xsl:with-param name="ref_oc" select="$s_l_o"/>
        					</xsl:call-template>
        				</xsl:when>
        				<xsl:otherwise>
        					<xsl:call-template name="cell_val">
        						<xsl:with-param name="val" select="js:adapt_str(string($ref_v))"/>
        						<xsl:with-param name="b_r" select="1"/>
        						<xsl:with-param name="lvl" select="$lvl"/>
        					</xsl:call-template>
        				</xsl:otherwise>
        			</xsl:choose>        
			    </xsl:otherwise>
			</xsl:choose>
		</fo:table-row>
		<!-- set level -->
		<xsl:if test="not(js:get_lvl() = $lvl)">
			<xsl:value-of select="js:set_lvl($lvl)"/>
		</xsl:if>	
		<!-- IF   documents are available display them -->
		<xsl:if test="count(sbtrpt:Relation/sbtrpt:Node[contains($doc_str, @Class)]) > 0">
			<xsl:call-template name="doc_inc">
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
		</xsl:if>
		<xsl:call-template name="rec_conf">
			<xsl:with-param name="lvl" select="$lvl"/>
		</xsl:call-template>
	</xsl:template>

<!-- document node -->
	<xsl:template name="rec_doc">
		<xsl:param name="lvl"/>
		<js:do_inc/>
		<fo:table-row>
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="js:get_num()"/>
				<xsl:with-param name="b_l" select="1"/>
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="'X'"/>
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
			<xsl:call-template name="cell_val">
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="'D'"/>
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="sbtrpt:Field[@Name='DocumentName']"/>
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
			<!--<xsl:variable name="revision">
				<xsl:if test="$m0BomType = 'LcsWorking'">
					<xsl:value-of select="sbtrpt:Field[@Name='Revision']"/>
				</xsl:if>
			</xsl:variable> 
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="$revision"/>
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template> -->
			<xsl:call-template name="cell_val">
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
			<xsl:call-template name="cell_val">
				<xsl:with-param name="val" select="sbtrpt:Field[@Name='DocumentTitle']"/>
				<xsl:with-param name="lvl" select="$lvl"/>
				<xsl:with-param name="b_r" select="1"/>
				<xsl:with-param name="n_c_s" select="5"/>
			</xsl:call-template>
		</fo:table-row>
		<xsl:if test="not(js:get_lvl() = $lvl)">
			<xsl:value-of select="js:set_lvl($lvl)"/>
		</xsl:if>
	</xsl:template>

</xsl:stylesheet>
