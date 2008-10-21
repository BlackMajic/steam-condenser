<?php
/**
 * @author Sebastian Staudt
 * @license http://www.opensource.org/licenses/bsd-license.php New BSD License
 * @package Steam Condenser (PHP)
 * @subpackage SteamPacket
 * @version $Id$
 */

require_once "steam/packets/SteamPacket.php";

/**
 * @package Steam Condenser (PHP)
 * @subpackage SteamPacket
 */
class A2A_RULES_ResponsePacket extends SteamPacket
{	
	/**
	 * @var String[]
	 */
	private $rulesArray;

	/**
	 * @param String $contentData
	 */
	public function __construct($contentData)
	{
		if(empty($contentData))
		{
			throw new Exception("Wrong formatted A2A_RULES response packet.");
		}
		parent::__construct(SteamPacket::A2A_RULES_RESPONSE_HEADER);
		
		$contentData = unpack("vrulesNumber/a*rulesData", $contentData);
		
		$tmpRulesArray = (explode("\0", $contentData["rulesData"]));
		for($x = 0; $x < sizeof($tmpRulesArray); $x++)
		{
			$this->rulesArray[$tmpRulesArray[$x]] = $tmpRulesArray[++$x];
		}
	}
	
	/**
	 * @return String[]
	 */
	public function getRulesArray()
	{
		return $this->rulesArray;
	}
}
?>