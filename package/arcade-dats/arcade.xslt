<?xml version="1.0"?>
<xsl:stylesheet version="1.1" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

   <!-- sortie texte -->
   <xsl:output method="text" indent="yes" />

   <!-- param lastmamexml -->
   <xsl:param name="lastmamexml"/>
 
   <!-- remplacement des espaces en %20 dans le nom de mamedatefile -->
   <xsl:variable name="finallastmamexml">
      <xsl:call-template name="string-replace-all">
         <xsl:with-param name="text" select="$lastmamexml" />
         <xsl:with-param name="replace" select="' '" />
         <xsl:with-param name="by" select="'%20'" />
      </xsl:call-template>
   </xsl:variable>

   <!-- chargement donnees xml last mame -->
   <xsl:variable name="lastmame" select="document($finallastmamexml)/mame"/>

   <!-- fonction string-replace-all pour remplacer replace de xslt 2.0 incompatible avec xsltproc limite a xslt 1.1 -->
   <xsl:template name="string-replace-all">
      <xsl:param name="text" />
      <xsl:param name="replace" />
      <xsl:param name="by" />
      <xsl:choose>
         <xsl:when test="$text = '' or $replace = ''or not($replace)" >
               <xsl:value-of select="$text" />
         </xsl:when>
         <xsl:when test="contains($text, $replace)">
               <xsl:value-of select="substring-before($text,$replace)" />
               <xsl:value-of select="$by" />
               <xsl:call-template name="string-replace-all">
                  <xsl:with-param name="text" select="substring-after($text,$replace)" />
                  <xsl:with-param name="replace" select="$replace" />
                  <xsl:with-param name="by" select="$by" />
               </xsl:call-template>
         </xsl:when>
         <xsl:otherwise>
               <xsl:value-of select="$text" />
         </xsl:otherwise>
      </xsl:choose>
   </xsl:template>

   <!-- template tableref vs -->
   <xsl:template name="tablerefvs">
      <xsl:param name="name" />
      
      <xsl:choose>
         <!--correspondance vs-->
         <xsl:when test="$name='vsraidbbay'"><xsl:value-of select="'bnglngby'"/></xsl:when>
         <xsl:when test="$name='vsrbibbal'"><xsl:value-of select="'rbibb'"/></xsl:when>
         <xsl:when test="$name='vsbattlecity'"><xsl:value-of select="'btlecity'"/></xsl:when>
         <xsl:when test="$name='vscastlevania'"><xsl:value-of select="'cstlevna'"/></xsl:when>
         <xsl:when test="$name='vsclucluland'"><xsl:value-of select="'cluclu'"/></xsl:when>
         <xsl:when test="$name='vsdrmario'"><xsl:value-of select="'drmario'"/></xsl:when>
         <xsl:when test="$name='vsduckhunt'"><xsl:value-of select="'duckhunt'"/></xsl:when>
         <xsl:when test="$name='vsexcitebike'"><xsl:value-of select="'excitebk'"/></xsl:when>
         <xsl:when test="$name='vsfreedomforce'"><xsl:value-of select="'vsfdf'"/></xsl:when>
         <xsl:when test="$name='vsgoonies'"><xsl:value-of select="'goonies'"/></xsl:when>
         <xsl:when test="$name='vsgradius'"><xsl:value-of select="'vsgradus'"/></xsl:when>
         <xsl:when test="$name='vsgumshoe'"><xsl:value-of select="'vsgshoe'"/></xsl:when>
         <xsl:when test="$name='vshogansalley'"><xsl:value-of select="'hogalley'"/></xsl:when>
         <xsl:when test="$name='vsiceclimber'"><xsl:value-of select="'iceclimb'"/></xsl:when>
         <xsl:when test="$name='vsmachrider'"><xsl:value-of select="'nvs_machrider'"/></xsl:when>
         <xsl:when test="$name='vsmightybomjack'"><xsl:value-of select="'nvs_mightybj'"/></xsl:when>
         <xsl:when test="$name='vsninjajkun'"><xsl:value-of select="'jajamaru'"/></xsl:when>
         <xsl:when test="$name='vspinball'"><xsl:value-of select="'vspinbal'"/></xsl:when>
         <xsl:when test="$name='vsplatoon'"><xsl:value-of select="'nvs_platoon'"/></xsl:when>
         <xsl:when test="$name='vsslalom'"><xsl:value-of select="'vsslalom'"/></xsl:when>
         <xsl:when test="$name='vssoccer'"><xsl:value-of select="'vssoccer'"/></xsl:when>
         <xsl:when test="$name='vsstarluster'"><xsl:value-of select="'starlstr'"/></xsl:when>
         <xsl:when test="$name='vssmgolf'"><xsl:value-of select="'smgolf'"/></xsl:when>
         <xsl:when test="$name='vssmgolfla'"><xsl:value-of select="'ladygolf'"/></xsl:when>
         <xsl:when test="$name='vssmb'"><xsl:value-of select="'suprmrio'"/></xsl:when>
         <xsl:when test="$name='vssuperskykid'"><xsl:value-of select="'vsskykid'"/></xsl:when>
         <xsl:when test="$name='vssuperxevious'"><xsl:value-of select="'supxevs'"/></xsl:when>
         <xsl:when test="$name='vstetris'"><xsl:value-of select="'vstetris'"/></xsl:when>
         <xsl:when test="$name='vstkoboxing'"><xsl:value-of select="'tkoboxng'"/></xsl:when>
         <xsl:when test="$name='vstopgun'"><xsl:value-of select="'topgun'"/></xsl:when>

         <xsl:otherwise><xsl:value-of select="$name"/></xsl:otherwise>
      </xsl:choose>
   </xsl:template>

   <!-- template tableref fba-->
   <xsl:template name="tablereffba">
      <xsl:param name="name" />
      
      <!--correspondance fba-->
      <xsl:choose>
         <xsl:when test="$name='getstar'"><xsl:value-of select="'grdian'"/></xsl:when>
         <xsl:when test="$name='battlegc'"><xsl:value-of select="'bgareggahk'"/></xsl:when>
         <xsl:when test="$name='battlegb'"><xsl:value-of select="'bgaregganv'"/></xsl:when>
         <xsl:when test="$name='battleg'"><xsl:value-of select="'bgareggacn'"/></xsl:when>
         <xsl:when test="$name='battlega'"><xsl:value-of select="'bgaregga'"/></xsl:when>
         <xsl:when test="$name='getstarb'"><xsl:value-of select="'getstarb1'"/></xsl:when>
         <xsl:when test="$name='gtstarba'"><xsl:value-of select="'getstarb2'"/></xsl:when>
         <xsl:when test="$name='qtono2'"><xsl:value-of select="'qtono2j'"/></xsl:when>
         <xsl:when test="$name='wc90'"><xsl:value-of select="'twcup90'"/></xsl:when>
         <xsl:when test="$name='wc90a'"><xsl:value-of select="'twcup90b'"/></xsl:when>
         <xsl:when test="$name='wc90t'"><xsl:value-of select="'twcup90t'"/></xsl:when>

         <xsl:otherwise><xsl:value-of select="$name"/></xsl:otherwise>
      </xsl:choose>
   </xsl:template>

   <!-- template datafile or mame-->
   <xsl:template match="datafile|mame">
      <!--nom zip-->
      <xsl:value-of select="'name'" /> 
      <xsl:value-of select="'|'"/>
      <!--nom du jeu -->
      <xsl:value-of select="'description'" /> 
      <xsl:value-of select="'|'"/>
      <!--type du jeu -->
      <xsl:value-of select="'type'" /> 
      <xsl:value-of select="'|'"/>
      <!--driver du jeu ref lastmame-->
      <xsl:value-of select="'sourcefile'" /> 
      <xsl:value-of select="'|'"/>
      <!--rotate du jeu ref lastmame-->
      <xsl:value-of select="'rotate'" /> 
      <xsl:value-of select="'|'"/>
      <!--bios du jeu-->
      <xsl:value-of select="'bios'" /> 
      <xsl:value-of select="'|'"/>
      <!--parent du jeu-->
      <xsl:value-of select="'parent'" /> 
      <xsl:value-of select="'|'"/>
      <!--status du jeu-->
      <xsl:value-of select="'status'" /> 
      <xsl:value-of select="'&#10;'"/>

	   <xsl:apply-templates select="game">
         <xsl:with-param name="isfba" select="substring(header/name,1,8)='FB Alpha'" />
      </xsl:apply-templates>
      <xsl:apply-templates select="machine" />
   </xsl:template>

   <!-- template game pour fichier dat -->
   <xsl:template match="game|machine">
      <xsl:param name="isfba" />

      <!-- #################Récupération données################# -->
      <!--type du jeu-->
      <xsl:variable name="type">
         <xsl:choose>
            <xsl:when test="@isbios='yes'">
               <xsl:value-of select="'bios'"/>
            </xsl:when>
            <xsl:when test="@isdevice='yes'">
               <xsl:value-of select="'device'"/>
            </xsl:when>
            <xsl:when test="@runnable='no'">
               <xsl:value-of select="'not-runnable'"/>
            </xsl:when>
            <xsl:when test="@cloneof">
               <xsl:value-of select="'clone'"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="'parent'"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:variable>
      <!--bios du jeu-->
      <xsl:variable name="bios">
         <xsl:choose>
            <xsl:when test="not(@cloneof)"> <!--pas un clone-->
               <xsl:value-of select="@romof" />
            </xsl:when>
            <xsl:otherwise> <!--clone-->
               <xsl:variable name="pere" select="@romof" />
               <xsl:choose>
                  <xsl:when test="../game[@name=$pere]">
                     <xsl:value-of select="../game[@name=$pere]/@romof" />
                  </xsl:when>
                  <xsl:when test="../machine[@name=$pere]">
                     <xsl:value-of select="../machine[@name=$pere]/@romof" />
                  </xsl:when>
               </xsl:choose>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:variable>
      <!--pere du jeu-->
      <xsl:variable name="pere">
         <xsl:if test="$type='clone'">
            <xsl:value-of select="@cloneof" /> 
         </xsl:if>
      </xsl:variable>
      <!--ref temporaire du jeu lastmame-->
      <xsl:variable name="lastmametmpref"> 
         <xsl:call-template name="tablerefvs">
            <xsl:with-param name="name" select="@name" />
         </xsl:call-template>
      </xsl:variable>
      <!--ref du jeu lastmame (jeu parent si la ref est introuvable)-->
      <xsl:variable name="lastmameref"> 
         <xsl:choose> 
            <xsl:when test="$lastmame/machine[@name=$lastmametmpref]">
               <xsl:value-of select="$lastmametmpref"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:call-template name="tablerefvs">
                  <xsl:with-param name="name" select="@cloneof" />
               </xsl:call-template>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:variable>
      <!--jeu fba-->      
      <xsl:variable name="lastmamerefexists" select="$lastmame/machine[@name=$lastmameref]" />
      <xsl:variable name="finalref"> 
         <xsl:choose>          
            <xsl:when test="not($isfba) or $lastmamerefexists">
               <xsl:value-of select="$lastmameref"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:call-template name="tablereffba">
                  <xsl:with-param name="name" select="@name" />
               </xsl:call-template>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:variable>
      <!--jeu lastmame-->
      <xsl:variable name='lastmamegame' select="$lastmame/machine[@name=$finalref]" />
      <!--sourcefile du jeu ref lastmame-->
      <xsl:variable name="lastmamesourcefile"> 
         <xsl:value-of select="$lastmamegame/@sourcefile" />
      </xsl:variable>
      <!--rotate du jeu ref lastmame-->
      <xsl:variable name="lastmamerotate"> 
         <xsl:value-of select="$lastmamegame/display/@rotate" />
      </xsl:variable>

      <!-- #################Resultats définitifs################# -->
      <!--nom zip du jeu-->
      <xsl:value-of select="@name" /> 
      <xsl:value-of select="'|'"/>
      <!--description final du jeu -->
      <xsl:value-of select="description"/>
      <xsl:value-of select="'|'"/>
      <!--type final du jeu -->
      <xsl:value-of select="$type"/>
      <xsl:value-of select="'|'"/>
      <!--sourcefile du jeu ref lastmame-->
      <xsl:value-of select="$lastmamesourcefile" /> 
      <xsl:value-of select="'|'"/>
      <!--rotate du jeu ref lastmame-->
      <xsl:value-of select="$lastmamerotate" /> 
      <xsl:value-of select="'|'"/>
      <!--bios du jeu-->
      <xsl:value-of select="$bios" /> 
      <xsl:value-of select="'|'"/>
      <!--parent du jeu-->
      <xsl:value-of select="$pere" /> 
      <xsl:value-of select="'|'"/>
      <!--status du jeu-->
      <xsl:value-of select="driver/@status" /> 
      <xsl:value-of select="'&#10;'"/>
   </xsl:template>

</xsl:stylesheet>
