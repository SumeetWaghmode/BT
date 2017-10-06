<?xml version="1.0" encoding="UTF-8"?>
<!--
	title: MD-Report: Multi-level parts list
	desc.: xml 2 csv

	time: 24.05.2011
-->
<!DOCTYPE stylesheet [
	<!ENTITY eol '<xsl:text>
</xsl:text>'>
	<!ENTITY sep '<xsl:text>;</xsl:text>'>
]>
<xsl:stylesheet version="1.0"
	xmlns:fo="http://www.w3.org/1999/XSL/Format"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:sbtrpt="http://mrtplm.com/pdm/xmlrep"
	xmlns:rptsup="http://mrtplm.com/pdm/xmlrep-support"
	xmlns:xalan="http://xml.apache.org/xalan"
	xmlns:str="http://exslt.org/strings"
	xmlns:js="ext1"
	extension-element-prefixes="js">

	<!--xsl:output indent="yes" method="xml" encoding="UTF-8"/-->
	<xsl:output indent="no" method="text" encoding="iso-8859-1"/>

	<xalan:component prefix="js" elements="init add_result set_specialline" functions="j_test generate_output get_specialline() escape_quotes">
		<xalan:script lang="javascript">
			var levelarray;
			var specialline = 0;

			function init()
			{
				levelarray = new Array(100000);
				for (var i = 0; levelarray.length > i; i++)
				{
					levelarray[i] = new Array("");
				}
				
				return null;
			}

			function j_test()
			{
				return "test";
			}

			function set_specialline(input)
			{
				specialline=input;
				return null;
			}

			function get_specialline()
			{
				return specialline;
			}


			function escape_quotes(text)
			{
				return text.replace(/"/g, "\"\"");
			}
			
			
			function add_result(level, linetext)
			{
				if(linetext.charAt(0)!= ";")
				{
					if(parseInt(level) > 1)
					{
							var numPointStr = "";
							
							for(var i = 1; i &lt; parseInt(level); i++)
							{
								numPointStr = numPointStr + ".";
							}
							
							levelarray[0].push(numPointStr.concat(linetext));
							return null;
					}
					else
					{
						levelarray[0].push(linetext);
						return null;
					}
				}
				else
				{
					levelarray[0].push(linetext);
					return null;
				}
			}


			function generate_output()
			{
				var str_output="";
				//str_output = str_output + "test";

	
				for (var i = 0; i &lt; levelarray.length; i++)
					{
						//str_output = str_output + " Gruppe" + i +": " ;
						for (var y = 0;y &lt; levelarray[i].length; y++)
						{
							if(levelarray[i][y] != "")
							{
								str_output = str_output + levelarray[i][y];
								str_output = str_output + "§$";
						}	}
					}
					
				return str_output;
			}

		</xalan:script>
	</xalan:component>



	<xsl:param name="l0ReportLocale"/>
	<xsl:param name="m0LastRevComplete"/>
	<xsl:param name="m0ProcessingLvl"/>
	<xsl:param name="m0InclPCBStruc"/>
	
	<!--xsl:param name="l0ReportLocale"/>
	<xsl:param name="m0LastRevComplete"/>
	<xsl:param name="m0ProcessingLvl"/>
	<xsl:param name="m0InclPCBStruc" select="'+'"/-->


	<xsl:variable name="doc_str" select="'dclass_dev,dclass_manu,dclass_UaD,dclass_MaP'"/>

	<xsl:variable name="doc_rel_str" select="'J0DocStr,l0DocPrt,PartDoc,l0PrDcLv'"/>
	<xsl:variable name="part_rel_str" select="'l0AuRvRv,AsRevRev'"/>
	
	<xsl:variable name="reportDate">
		<xsl:value-of select="sbtrpt:ResultSet/@Date"/>
	</xsl:variable>

	<xsl:variable name="reportTime">
		<xsl:value-of select="sbtrpt:ResultSet/@Time"/>
	</xsl:variable>

	<xsl:variable name="reportTitleHeading">
		<xsl:choose>			<xsl:when test="not($m0ProcessingLvl = '1')">				<xsl:choose>					<xsl:when test="$l0ReportLocale = 'DE'">						<xsl:value-of select="'Mehrebenen-Teileliste (mit Dokumenten)'"/>					</xsl:when>					<xsl:when test="$l0ReportLocale = 'EN'">						<xsl:value-of select="'Multi-level parts list (with documents)'"/>					</xsl:when>								</xsl:choose>							</xsl:when>						<xsl:when test="$m0ProcessingLvl = '1'">							<xsl:choose>									<xsl:when test="$l0ReportLocale = 'DE'">											<xsl:value-of select="'Einebenen-Teileliste (mit Dokumenten)'"/>											</xsl:when>										<xsl:when test="$l0ReportLocale = 'EN'">											<xsl:value-of select="'Single-level parts list (with documents)'"/>											</xsl:when>									</xsl:choose>									</xsl:when>		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="PCBYesNo">
		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'EN'">
				<xsl:choose>
					<xsl:when test="$m0InclPCBStruc = 'true'">
						<xsl:value-of select="'Yes'"/>
					</xsl:when>

					<xsl:when test="$m0InclPCBStruc = 'false'">
						<xsl:value-of select="'No'"/>
					</xsl:when>

				</xsl:choose>
			</xsl:when>
		</xsl:choose>

		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'DE'">
				<xsl:choose>
					<xsl:when test="$m0InclPCBStruc = 'true'">
						<xsl:value-of select="'Ja'"/>
					</xsl:when>
					<xsl:when test="$m0InclPCBStruc = 'false'">
						<xsl:value-of select="'Nein'"/>
					</xsl:when>
				</xsl:choose>
			</xsl:when>
		</xsl:choose>
	</xsl:variable>


	<xsl:variable name="ConfigContext">
		<xsl:value-of select="$m0LastRevComplete"/>		<!--xsl:choose>
			<xsl:when test="$l0ReportLocale = 'DE'">
				<xsl:choose>
					<xsl:when test="$m0LastRevComplete = '+'">
						<xsl:value-of select="'Letzte Revision und Definitiv'"/>
					</xsl:when>

					<xsl:when test="$m0LastRevComplete = '-'">
						<xsl:value-of select="'Letzte Revision'"/>
					</xsl:when>

				</xsl:choose>
			</xsl:when>
		</xsl:choose>

		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'EN'">
				<xsl:choose>
					<xsl:when test="$m0LastRevComplete = '+'">
						<xsl:value-of select="'Last Revision and Complete'"/>
					</xsl:when>
					<xsl:when test="$m0LastRevComplete = '-'">
						<xsl:value-of select="'Last Revision'"/>
					</xsl:when>
				</xsl:choose>
			</xsl:when>
		</xsl:choose-->
	</xsl:variable>


	<xsl:template match="sbtrpt:ResultSet">
		<js:init/>
		<xsl:apply-templates/>
	</xsl:template>

	<xsl:template match="sbtrpt:Node">
		<xsl:call-template name="header"/>
		<xsl:call-template name="body"/>
		<xsl:call-template name="output"/>
		<xsl:call-template name="footer"/>
	</xsl:template>

	<xsl:template name="output">
		<xsl:param name="isseperatorline"/>
			<xsl:for-each select="str:split(js:generate_output(), '§$')">
				<xsl:value-of select='node()'/>
				&eol;
			</xsl:for-each>
	</xsl:template>

	<xsl:template name="header">
		<xsl:variable name="l0TechInfo">
			<xsl:value-of select="sbtrpt:Field[@Name='l0TechInfo']" />
		</xsl:variable>
		
		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'DE'">

				&eol;<xsl:value-of select="$reportTitleHeading"/>&sep;
				&eol;
				<xsl:text>Einstellungen:</xsl:text>&sep;
				&eol;
				<xsl:text> Anzahl Ebenen: </xsl:text> <xsl:value-of select="$m0ProcessingLvl"/>&sep;
				&eol;
				<xsl:text> Inkl. LP-Struktur: </xsl:text> <xsl:value-of select="$PCBYesNo"/>&sep;
				&eol;
				<xsl:text> Konfigurationskontext: </xsl:text> <xsl:value-of select="$ConfigContext"/>&sep;
				&eol;&eol;

				<xsl:text>Sachnummer</xsl:text>&sep;
				<xsl:text>Rev.</xsl:text>&sep;
				<xsl:text>Status</xsl:text>&sep;
				<xsl:text>Teileart</xsl:text>&sep;
				<xsl:text>Bezeichnung</xsl:text>&sep;
				<xsl:text>Äquiv. Teile Nr.</xsl:text>&sep;
				<xsl:text>Technische Info</xsl:text>&sep;
				<xsl:text>Freig. Datum</xsl:text>&sep;
				&eol;
				
				<xsl:value-of select="sbtrpt:Field[@Name='PartNumber']"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='Revision']"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='LifeCycleState']"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='PartType']"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='Nomenclature']"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='L0DocCode']"/>&sep;
				<xsl:value-of select="js:escape_quotes($l0TechInfo)"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='l0ReleaseDate']"/>&sep;
				&eol;

				&eol;
				<xsl:text>Ebene</xsl:text> &sep;
				<xsl:text>Pos.-Nr.</xsl:text> &sep;
				<xsl:text>Menge</xsl:text> &sep;
				<xsl:text>Einheit</xsl:text> &sep;
				<xsl:text>Sachnummer</xsl:text> &sep;
				<xsl:text>Rev.</xsl:text> &sep;
				<xsl:text>Status</xsl:text> &sep;
				<xsl:text>Teile-/Dokumentenart</xsl:text> &sep;
				<xsl:text>Bezeichnung de</xsl:text> &sep;
				<xsl:text>Bezeichnung en</xsl:text> &sep;
				<xsl:text>Technische Info</xsl:text> &sep;
				&eol;
			</xsl:when>

			<xsl:when test="$l0ReportLocale = 'EN'">
				&eol;<xsl:value-of select="$reportTitleHeading"/>&sep;
				&eol;
				<xsl:text>Preferences:</xsl:text>&sep;
				&eol;
				<xsl:text> Number of Level: </xsl:text> <xsl:value-of select="$m0ProcessingLvl"/>&sep;
				&eol;
				<xsl:text> Incl. PCBA: </xsl:text> <xsl:value-of select="$PCBYesNo"/>&sep;
				&eol;
				<xsl:text> Configuration Context: </xsl:text> <xsl:value-of select="$ConfigContext"/>&sep;
				&eol;&eol;

				<xsl:text>Identification no.</xsl:text>&sep;
				<xsl:text>Rev.</xsl:text>&sep;
				<xsl:text>Status</xsl:text>&sep;
				<xsl:text>Part type</xsl:text>&sep;
				<xsl:text>Designation</xsl:text>&sep;
				<xsl:text>Equiv. part no.</xsl:text>&sep;
				<xsl:text>Technical info</xsl:text>&sep;
				<xsl:text>Release date</xsl:text>&sep;
				&eol;

				<xsl:value-of select="sbtrpt:Field[@Name='PartNumber']"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='Revision']"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='LifeCycleState']"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='PartType']"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='Nomenclature']"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='L0DocCode']"/>&sep;
				<xsl:value-of select="js:escape_quotes($l0TechInfo)"/>&sep;
				<xsl:value-of select="sbtrpt:Field[@Name='l0ReleaseDate']"/>&sep;
				&eol;

				&eol;
				<xsl:text>Level</xsl:text> &sep;
				<xsl:text>Part ref.</xsl:text> &sep;
				<xsl:text>Quantity</xsl:text> &sep;
				<xsl:text>Unit</xsl:text> &sep;
				<xsl:text>Identification no.</xsl:text> &sep;
				<xsl:text>Rev.</xsl:text> &sep;
				<xsl:text>Status</xsl:text> &sep;
				<xsl:text>Part-/documenttype</xsl:text> &sep;
				<xsl:text>Designation de</xsl:text> &sep;
				<xsl:text>Designation en</xsl:text> &sep;
				<xsl:text>Technical info</xsl:text> &sep;
				&eol;
			</xsl:when>
		</xsl:choose>

		<xsl:call-template name="doc_inc"/>
	</xsl:template>


	<xsl:template name="body">
		<xsl:call-template name="recursion_controller" />
	</xsl:template>

	
	
	<xsl:template name="recursion_controller">
		<xsl:for-each select="sbtrpt:Relation[contains($part_rel_str, @Class)]">		
			<xsl:call-template name="assm_inc" />
		</xsl:for-each>
	</xsl:template>
	

	<xsl:template name="doc_inc">
		<xsl:for-each select="sbtrpt:Relation[contains($doc_rel_str, @Class)]/sbtrpt:Node">
			<xsl:call-template name="rec_doc" />	
		</xsl:for-each>
	</xsl:template>

	<xsl:template name="assm_inc">
		<xsl:for-each select="sbtrpt:Node">
			<xsl:call-template name="rec_assm" />
		</xsl:for-each>
	</xsl:template>


	<xsl:template name="rec_doc">
		<xsl:param name="tmpline">		
			
			<xsl:value-of select="concat(
						'','&#59;', 
				'&#34;',../sbtrpt:Field[@Name='FindNumber'],'&#34;','&#59;', 
				'&#34;',../sbtrpt:Field[@Name='DisplayQuantity'],'&#34;','&#59;', 
				'&#34;',../sbtrpt:Field[@Name='UnitOfMeasure'],'&#34;','&#59;', 
				'&#34;',sbtrpt:Field[@Name='DocumentName'],'&#34;','&#59;', 
				'&#34;',sbtrpt:Field[@Name='Revision'],'&#34;','&#59;', 
				'&#34;',sbtrpt:Field[@Name='LifeCycleState'],'&#34;','&#59;', 
				'&#34;',sbtrpt:Field[@Name='DisplayedType'],'&#34;','&#59;', 
				'&#34;',sbtrpt:Field[@Name='DocumentTitle'],'&#34;','&#59;', 
				'&#34;',sbtrpt:Field[@Name='l0DocumentTitleEng'],'&#34;','&#59;',
				'&#34;','---','&#34;','&#59;')"/>
		</xsl:param>

		<xsl:value-of select="js:add_result('0', $tmpline)"/>
	</xsl:template>
	
	
	
	<xsl:template name="rec_assm">
		<xsl:variable name="fnvalue">
			<xsl:value-of select="../sbtrpt:Field[@Name='FindNumber']" />
		</xsl:variable>
		
		<xsl:variable name="lvl">
			<xsl:value-of select="sbtrpt:Field[@Name='Level']" />
		</xsl:variable>

		<xsl:variable name="l0TechInfo">
			<xsl:value-of select="sbtrpt:Field[@Name='l0TechInfo']" />
		</xsl:variable>
		
		<xsl:choose>
			<xsl:when test="$fnvalue = ''">
				<xsl:variable name="tmpline">
					<xsl:value-of select="concat(
						sbtrpt:Field[@Name='Level'],'&#59;', 
						'&#34;',../sbtrpt:Field[@Name='ReferenceDesignator'],'&#34;','&#59;', 
						'&#34;',../sbtrpt:Field[@Name='DisplayQuantity'],'&#34;','&#59;', 
						'&#34;',../sbtrpt:Field[@Name='UnitOfMeasure'],'&#34;','&#59;',  
						'&#34;',sbtrpt:Field[@Name='PartNumber'],'&#34;','&#59;',
						'&#34;',sbtrpt:Field[@Name='Revision'],'&#34;','&#59;', 
						'&#34;',sbtrpt:Field[@Name='LifeCycleState'],'&#34;','&#59;', 
						'&#34;',sbtrpt:Field[@Name='PartType'],'&#34;','&#59;', 
						'&#34;',sbtrpt:Field[@Name='Nomenclature2'],'&#34;','&#59;',
						'&#34;',sbtrpt:Field[@Name='Nomenclature'],'&#34;','&#59;',
						'&#34;',js:escape_quotes($l0TechInfo),'&#34;','&#59;')"/>
				</xsl:variable>
			
				<xsl:value-of select="js:add_result($lvl, $tmpline)"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:variable name="tmpline">
					<xsl:value-of select="concat(
						sbtrpt:Field[@Name='Level'],'&#59;', 
						'&#34;',../sbtrpt:Field[@Name='FindNumber'],'&#34;','&#59;', 
						'&#34;',../sbtrpt:Field[@Name='DisplayQuantity'],'&#34;','&#59;', 
						'&#34;',../sbtrpt:Field[@Name='UnitOfMeasure'],'&#34;','&#59;',  
						'&#34;',sbtrpt:Field[@Name='PartNumber'],'&#34;','&#59;',
						'&#34;',sbtrpt:Field[@Name='Revision'],'&#34;','&#59;', 
						'&#34;',sbtrpt:Field[@Name='LifeCycleState'],'&#34;','&#59;', 
						'&#34;',sbtrpt:Field[@Name='PartType'],'&#34;','&#59;', 
						'&#34;',sbtrpt:Field[@Name='Nomenclature2'],'&#34;','&#59;',
						'&#34;',sbtrpt:Field[@Name='Nomenclature'],'&#34;','&#59;',
						'&#34;',js:escape_quotes($l0TechInfo),'&#34;','&#59;')"/>
				</xsl:variable>
				
				<xsl:value-of select="js:add_result($lvl, $tmpline)"/>
			</xsl:otherwise>
		</xsl:choose>
		
		<xsl:call-template name="doc_inc" />
		
		<xsl:call-template name="rec_conf" />
	</xsl:template>


	<xsl:template name="rec_conf">
			<xsl:call-template name="recursion_controller" />
	</xsl:template>

	
	<xsl:template name="footer">
		<xsl:choose>
			<xsl:when test="$l0ReportLocale = 'DE'">
				<xsl:text>&#xA9; Siemens Switzerland Ltd, Nur für internen Gebrauch</xsl:text>&sep;
			</xsl:when>
			<xsl:when test="$l0ReportLocale = 'EN'">
				<xsl:text>&#xA9; Siemens Switzerland Ltd, Restricted</xsl:text>&sep;
			</xsl:when>
		</xsl:choose>

		<xsl:value-of select="$reportDate"/> 
		<xsl:text>, </xsl:text>
		<xsl:value-of select="$reportTime"/> 
	</xsl:template>

</xsl:stylesheet>
