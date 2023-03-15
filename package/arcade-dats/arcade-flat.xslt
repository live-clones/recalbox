<?xml version="1.0"?>
<xsl:stylesheet version="1.1" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

   <!-- sortie texte -->
   <xsl:output method="text" indent="yes" />
 
   <!-- template datafile or mame-->
   <xsl:template match="datafile|mame">
	   <xsl:apply-templates select="game" />
      <xsl:apply-templates select="machine" />
   </xsl:template>

   <!-- template game pour fichier dat -->
   <xsl:template match="game|machine">
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
      <!--pere du jeu-->
      <xsl:variable name="pere">
         <xsl:if test="$type='clone'">
            <xsl:value-of select="@cloneof" /> 
         </xsl:if>
      </xsl:variable>
      
      <!-- #################Resultats définitifs################# -->
      <!--nom zip du jeu-->
      <xsl:value-of select="@name" /> 
      <xsl:value-of select="'|'"/>
      <!--parent du jeu-->
      <xsl:value-of select="$pere" /> 
      <!--roms et md4-->
      <xsl:for-each select="rom">
         <xsl:value-of select="'|'"/>
         <xsl:value-of select="@name"/>
         <xsl:value-of select="'|'"/>
         <xsl:value-of select="@crc"/>
      </xsl:for-each>
      <xsl:value-of select="'&#10;'"/>

   </xsl:template>

</xsl:stylesheet>
