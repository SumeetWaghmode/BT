<?xml version="1.0" encoding="UTF-8"?>

<!--
	title: sbt customized bill of material (bom)
	desc.: xml 2 csv 4 excel

	version: spec v1.4 (update after phonecall) compliant; lib excluded; sbt ns
	time: 06.12.2007 17:00
-->

<!DOCTYPE stylesheet [
	<!ENTITY eol '<xsl:text>
</xsl:text>'>
	<!ENTITY sep '<xsl:text>";"</xsl:text>'>
]>

<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:sbtrpt="http://mrtplm.com/pdm/xmlrep"
    xmlns:rptsup="http://mrtplm.com/pdm/xmlrep-support"
	xmlns:xalan="http://xml.apache.org/xalan"
	xmlns:js="ext1"
	extension-element-prefixes="js">

	<xsl:output indent="no" method="text" encoding="iso-8859-1"/>

<xsl:key name="assembly" match="sbtrpt:Node" use="@OBID"/>

	<xalan:component prefix="js" elements="do_inc set_lvl add_obid_ prt add_obid_doc" functions="numInString str_end reverse adapt_str get_num get_lvl chk_proc_prt chk_proc_doc str fig str_1">
		<xalan:script lang="javascript">
			var num = 0;
			var obids_prt = "";
			var obids_doc = "";
			var lvl = 0;

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
		</xalan:script>
	</xalan:component>

<!--	params  -->
	<xsl:param name="m0ProcessingLvl"/>         <!--        select="5" /> -->
	<xsl:param name="m0BomType"/>                   <!--      select="'LcsWorking'" /> -->
	<xsl:param name="m0MListStdCmpLV"/> 			 <!--	  select="'-'" /> -->
	<xsl:param name="m0seAltIdInBomFlag"/>           <!--    select="'-'" /> -->
	<xsl:param name="m0ForceSingleLine"/>            <!--   select="'+'" /> -->

	<!-- identification strings -->
	<!-- CR920 - add l0ElcPro to asmb_str -->
	<xsl:variable name="asmb_str" select="'Assembly,DocuPart,j9Pkg,j9Tool,l0AsmPrg,l0AsSoft,l0ElcPro'"/>
	<xsl:variable name="comp_str" select="'Cmponent,l0SbtCmp,j9Aux,j9Mat'"/>
	<xsl:variable name="doc_str" select="'dclass_dev,dclass_manu,dclass_UaD,dclass_MaP'"/>
	<xsl:variable name="auxr_str" select="'j9Pkg,j9Tool'"/>
	<xsl:variable name="auxm_str" select="'j9Aux,j9Mat'"/>
	<!-- configurationstring for relations between Assembly and Documents       -->
	<!-- (add relations class for displaying related Documents                  -->
	<xsl:variable name="ass2doc_relations" select="'l0DocPrt,PartDoc'"/>

	<!-- global var -->
	<xsl:variable name="sort_name">
		<xsl:choose>
			<xsl:when test="$m0seAltIdInBomFlag = '+'">
				<xsl:value-of select="'L0DocCode'"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="'PartNumber'"/>r
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="type_display">
		<xsl:choose>
			<xsl:when test="$m0BomType = 'LcsWorking'">
				<xsl:value-of select="'provisorisch'"/>
			</xsl:when>
			<xsl:when test="$m0BomType = 'LcsReleased'">
				<xsl:value-of select="'definitiv'"/>
			</xsl:when>
		</xsl:choose>
	</xsl:variable>

<!--	recursion controller -->
	<xsl:template name="rec_con">
		<xsl:param name="lvl"/>
		<xsl:if test="($lvl=0) and (count(sbtrpt:Relation/sbtrpt:Node[contains($doc_str, @Class)]) > 0)">
			<xsl:call-template name="doc_inc">
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
		</xsl:if>
		<xsl:for-each select="sbtrpt:Relation/sbtrpt:Node[contains($asmb_str, @Class) or contains($comp_str, @Class)]">
			<xsl:sort select="js:str_1(string(sbtrpt:Field[@Name=$sort_name]))" order="ascending"/>
			<xsl:sort select="format-number(string(sbtrpt:Field[@Name=$sort_name]), '0000000000')" order="ascending"/>
			<xsl:sort select="js:str(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl']))" order="ascending"/>
			<xsl:sort select="format-number(js:fig(string(../sbtrpt:Field[@Name='FindNumber']),string(../sbtrpt:Field[@Name='l0RefDesigListCl'])), '000')" order="ascending"/>
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
			(js:chk_proc_doc(string(@OBID)) = 1 ) or
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
						  	(../../sbtrpt:Field[@Name='Superseded'] = 'True') or
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

	<xsl:template match="text()"/>

	<xsl:template match="sbtrpt:ResultSet">
		<xsl:apply-templates/>
	</xsl:template>

<!-- making the geräte-software bom available by adding l0AsSoft here -->
	<xsl:template match="sbtrpt:ResultSet/sbtrpt:Node[@Class='LGProdkt' or @Class='Assembly' or @Class='l0AsSoft' or @Class='j9Pkg']">
		<xsl:call-template name="footer"/>
		<xsl:call-template name="body"/>
	</xsl:template>

	<xsl:template name="footer">
		<xsl:text>Erstellt: </xsl:text>
		<xsl:value-of select="../@Date"/>
		<xsl:text> @ </xsl:text>
		<xsl:value-of select="../@Time"/>
		<xsl:text>, </xsl:text>
		<xsl:value-of select="../@Creator"/>
		&sep;
		<xsl:text>Stückliste zu / BOM for </xsl:text>
		<xsl:value-of select="sbtrpt:Field[@Name='PartNumber']"/>
		&sep;
		<xsl:text>Freig.: </xsl:text>
		&sep;
		<xsl:value-of select="sbtrpt:Field[@Name='Nomenclature']"/>
		&sep;
		<xsl:text>Verantw. Stelle: </xsl:text>
		<xsl:value-of select="sbtrpt:Field[@Name='ResponsablePerson']"/>
		<xsl:text>, </xsl:text>
		<xsl:value-of select="sbtrpt:Field[@Name='LGRespPhone']"/>
		<xsl:text>, </xsl:text>
		<xsl:value-of select="sbtrpt:Field[@Name='LGRespUnit']"/>
		&sep;
		<xsl:text>Index: </xsl:text>
		<xsl:value-of select="sbtrpt:Field[@Name='Revision']"/>
		&sep;
		<xsl:text>Copyright 1996 by Siemens Building Technologies AG</xsl:text>
		&sep;
		<xsl:text>Nummer/Index: </xsl:text>
		<xsl:value-of select="$type_display"/>
		<xsl:text>";</xsl:text>
		&eol;
		&eol;
	</xsl:template>

	<xsl:template name="body">
		<xsl:text>"#";"Qty";"Un";"Lvl";"Ident.";"Rev.";"Stat.";"Nomenclature";"Tech. Info";"SMD";"F/N";"Ref."</xsl:text>
		&eol;
		<xsl:call-template name="rec_con">
			<xsl:with-param name="lvl" select="0"/>
		</xsl:call-template>
	</xsl:template>

<!--	recursion: part node -->
	<xsl:template name="rec_part">
		<xsl:param name="lvl"/>
		<xsl:variable name="s_l_o">
			<xsl:if test="contains(../sbtrpt:Field[@Name='l0RefDesigListCl'],',') 
						  or contains(../sbtrpt:Field[@Name='l0RefDesigListCl'],' ')
						  or contains(../sbtrpt:Field[@Name='FindNumber'],',')
						  or contains(../sbtrpt:Field[@Name='FindNumber'],' ')">
				<xsl:value-of select="'True'"/>
			</xsl:if>
		</xsl:variable>
		<js:do_inc/>
		<xsl:text>"</xsl:text>
		<xsl:value-of select="js:get_num()"/>
		&sep;
		<xsl:choose>
			<xsl:when test="../sbtrpt:Field[@Name='Quantity'] = 'l0req'">
				<xsl:text>req</xsl:text>
			</xsl:when>
			<xsl:when test="../sbtrpt:Field[@Name='Quantity'] = 'l0Kar'">
				<xsl:text>Box</xsl:text>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="../sbtrpt:Field[@Name='Quantity']"/>
			</xsl:otherwise>
		</xsl:choose>
		&sep;
		<xsl:value-of select="../sbtrpt:Field[@Name='UnitOfMeasure']"/>
		&sep;
		<xsl:choose>
			<xsl:when test="contains($auxm_str, @Class)">
				<xsl:text>M</xsl:text>
			</xsl:when>
			<xsl:when test="$lvl > 1">
				<xsl:value-of select="concat('_',$lvl)"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$lvl"/>
			</xsl:otherwise>
		</xsl:choose>
		&sep;
		<xsl:value-of select="sbtrpt:Field[@Name='PartNumber']"/>
		&sep;
		<xsl:if test="$m0BomType = 'LcsWorking'">
				<xsl:value-of select="sbtrpt:Field[@Name='Revision']"/>
		</xsl:if>
		&sep;
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
		&sep;
		<xsl:value-of select="sbtrpt:Field[@Name='Nomenclature']"/>
		&sep;
		<xsl:value-of select="sbtrpt:Field[@Name='l0TechInfo']"/>
		&sep;
		<xsl:if test="sbtrpt:Field[@Name='l0SMDevice'] = 'True'">
			<xsl:text>J</xsl:text>
		</xsl:if>
		&sep; 	
	    	<xsl:choose>
				<xsl:when test="../sbtrpt:Field[@Name='FindNumber'] != 0">
					<xsl:value-of select="../sbtrpt:Field[@Name='FindNumber']"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="../sbtrpt:Field[@Name='l0RefDesigListCl']"/>
				</xsl:otherwise>
			</xsl:choose>
		&sep;
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
		<xsl:choose>
			<xsl:when test="$s_l_o = 'True'">
				<xsl:value-of select="../sbtrpt:Field[@Name='l0RefDesigListCl']"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$ref_v"/>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:text>"</xsl:text>
		&eol;
		<xsl:if test="count(sbtrpt:Relation/sbtrpt:Node[contains($doc_str, @Class)]) > 0">
			<xsl:call-template name="doc_inc">
				<xsl:with-param name="lvl" select="$lvl"/>
			</xsl:call-template>
		</xsl:if>
		<xsl:call-template name="rec_conf">
			<xsl:with-param name="lvl" select="$lvl"/>
		</xsl:call-template>
	</xsl:template>

<!--	recursion: document node -->
	<xsl:template name="rec_doc">
		<xsl:param name="lvl"/>
		<js:do_inc/>
		<xsl:text>"</xsl:text>
		<xsl:value-of select="js:get_num()"/>
		&sep;
		<xsl:text>X</xsl:text>
		&sep;
		&sep;
		<xsl:text>D</xsl:text>
		&sep;
		<xsl:value-of select="sbtrpt:Field[@Name='DocumentName']"/>
		&sep;
		<xsl:if test="$m0BomType = 'LcsWorking'">
		<xsl:value-of select="sbtrpt:Field[@Name='Revision']"/>
		</xsl:if>
		&sep;
		&sep;
		<xsl:value-of select="sbtrpt:Field[@Name='DocumentTitle']"/>
		&sep;
		&sep;
		&sep;
		&sep;
		&sep;
		&sep;
		<xsl:text>"</xsl:text>
		&eol;
		<xsl:call-template name="rec_conf">
			<xsl:with-param name="lvl" select="$lvl"/>
		</xsl:call-template>
	</xsl:template>

</xsl:stylesheet>
